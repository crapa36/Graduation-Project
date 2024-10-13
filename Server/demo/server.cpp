#include <iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <array>
#include <vector>
#include <unordered_map>

#include "protocol.h"


#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")



enum COMP_TYPE { OP_ACCEPT, OP_RECV, OP_SEND };
class OVER_EXP {
public:
	WSAOVERLAPPED _over;
	WSABUF _wsabuf;
	char _send_buf[BUF_SIZE];
	COMP_TYPE _comp_type;
	OVER_EXP()
	{
		_wsabuf.len = BUF_SIZE;
		_wsabuf.buf = _send_buf;
		_comp_type = OP_RECV;
		ZeroMemory(&_over, sizeof(_over));
	}
	OVER_EXP(char* packet)
	{
		_wsabuf.len = packet[0];
		_wsabuf.buf = _send_buf;
		ZeroMemory(&_over, sizeof(_over));
		_comp_type = OP_SEND;
		memcpy(_send_buf, packet, packet[0]);
	}
};

enum S_STATE { ST_FREE, ST_INGAME };
class SESSION {
public:
	int _id;
	SOCKET _socket;
	S_STATE _state;
	OVER_EXP _recv_over;
	int _prev_remain;
	char _name[MAX_NAME_SIZE];
	float x, y, z;

	SESSION()
	{
		_id = -1;
		_socket = 0;
		x = 0.f;
		y = 0.f;
		z = 0.f;
		_name[0] = 0;
		_state = ST_FREE;
		_prev_remain = 0;
	}

	~SESSION() {}

	void do_recv()
	{
		DWORD recv_flag = 0;
		memset(&_recv_over._over, 0, sizeof(_recv_over._over));
		_recv_over._wsabuf.len = BUF_SIZE - _prev_remain;
		_recv_over._wsabuf.buf = _recv_over._send_buf + _prev_remain;
		WSARecv(_socket, &_recv_over._wsabuf, 1, 0, &recv_flag,
			&_recv_over._over, 0);
	}

	void do_send(void* packet)
	{
		OVER_EXP* sdata = new OVER_EXP{ reinterpret_cast<char*>(packet) };
		WSASend(_socket, &sdata->_wsabuf, 1, 0, 0, &sdata->_over, 0);
	}

	void send_login_info_packet()
	{
		SC_LOGIN_INFO_PACKET p;
		p.id = _id;
		p.size = sizeof(SC_LOGIN_INFO_PACKET);
		p.type = SC_LOGIN_INFO;
		p.x = x;
		p.y = y;
		p.z = z;
		do_send(&p);
	}

	void send_remove_player_packet(int c_id)
	{
		SC_REMOVE_PLAYER_PACKET p;
		p.id = c_id;
		p.size = sizeof(p);
		p.type = SC_REMOVE_PLAYER;
		do_send(&p);
	}

	void send_move_packet(int c_id);
	void send_add_player_packet(int c_id);
};

std::array<SESSION, MAX_USER> clients;

void SESSION::send_move_packet(int c_id)
{
	CS_PLAYER_MOVE_PACKET p;
	p.id = c_id;
	p.size = sizeof(CS_PLAYER_MOVE_PACKET);
	p.type = CS_PLAYER_MOVE;
	p.x = clients[c_id].x;
	p.y = clients[c_id].y;
	p.z = clients[c_id].z;
	do_send(&p);
}

void SESSION::send_add_player_packet(int c_id)
{
	SC_ADD_PLAYER_PACKET add_packet;
	add_packet.id = c_id;
	strcpy_s(add_packet.name, clients[c_id]._name);
	add_packet.size = sizeof(add_packet);
	add_packet.type = SC_ADD_PLAYER;
	add_packet.x = clients[c_id].x;
	add_packet.y = clients[c_id].y;
	add_packet.z = clients[c_id].z;
	do_send(&add_packet);
}

int get_new_client_id()
{
	for (int i = 0; i < MAX_USER; ++i) {
		if (clients[i]._state == ST_FREE)
			return i;
	}
	return -1;
}

void disconnect(int c_id)
{
	for (auto& pl : clients) {
		if (ST_INGAME != pl._state) continue;
		if (pl._id == c_id) continue;
		pl.send_remove_player_packet(c_id);
		std::cout << "for client number [" << pl._id << "], " << "Client [" << c_id << "] Disconnected\n";
	}
	closesocket(clients[c_id]._socket);
	clients[c_id]._state = ST_FREE;
}

void process_packet(int c_id, char* packet)
{
	switch (packet[1]) {
	case CS_LOGIN: {
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		strcpy_s(clients[c_id]._name, p->name);
		clients[c_id].send_login_info_packet();
		for (auto& pl : clients) {
			if (ST_INGAME != pl._state) continue;
			if (pl._id == c_id) continue;
			pl.send_add_player_packet(c_id);
			clients[c_id].send_add_player_packet(pl._id);
		}
		break;
	}
	case CS_PLAYER_MOVE: {
		CS_PLAYER_MOVE_PACKET* p = reinterpret_cast<CS_PLAYER_MOVE_PACKET*>(packet);
		clients[c_id].x = p->x;
		clients[c_id].y = p->y;
		clients[c_id].z = p->z;
		std::cout <<"clients["<< c_id << "] Pos : " << clients[c_id].x << ", " << clients[c_id].y << ", " << clients[c_id].z << std::endl;
		for (auto& cl : clients) {
			if (cl._state != ST_INGAME) continue;
			cl.send_move_packet(c_id);

		}
	}
	}
}


int main() {
	WSAData WSADATA;
	WSAStartup(MAKEWORD(2, 2), &WSADATA);
	SOCKET server_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(server_socket, SOMAXCONN);

	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);
	int client_id = 0;

	HANDLE h_iocp;
	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(server_socket), h_iocp, 9999, 0);
	std::unordered_map<int, SOCKET> client_socket;
	client_socket.insert(std::pair(0, WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)));
	OVER_EXP a_over;
	a_over._comp_type = OP_ACCEPT;
	AcceptEx(server_socket, client_socket[0], a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &a_over._over);
	int new_id = get_new_client_id();
	while (true) {
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &over, INFINITE);
		OVER_EXP* ex_over = reinterpret_cast<OVER_EXP*>(over);
		if (FALSE == ret) {
			if (ex_over->_comp_type == OP_ACCEPT) std::cout << "Accept Error";
			else {
				std::cout << "GQCS Error on client[" << key << "]\n";
				disconnect(static_cast<int>(key));
				client_socket.erase(static_cast<int>(key));
				if (ex_over->_comp_type == OP_SEND) delete ex_over;
				continue;
			}
		}

		if ((0 == num_bytes) && ((ex_over->_comp_type == OP_RECV) || (ex_over->_comp_type == OP_SEND))) {
			disconnect(static_cast<int>(key));
			if (ex_over->_comp_type == OP_SEND) delete ex_over;
			continue;
		}
		
		switch (ex_over->_comp_type) {
		case OP_ACCEPT: {
			
			if (new_id != -1) {
				clients[new_id]._id = new_id;
				clients[new_id]._state = ST_INGAME;
				clients[new_id]._socket = client_socket[new_id];
				clients[new_id]._prev_remain = 0;
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_socket[new_id]), h_iocp, new_id, 0);
				clients[new_id].do_recv();
				std::cout << "Client [" << new_id << "] Connected\n";
				new_id = get_new_client_id();
				client_socket.insert(std::pair(new_id, WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)));
			}
			else
			{
				std::cout << "no more space" << std::endl;
			}
			ZeroMemory(&a_over._over, sizeof(a_over._over));
			new_id = get_new_client_id();
			AcceptEx(server_socket, client_socket[new_id], a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &a_over._over);
			break;
		}
		case OP_RECV: {
			int remain_data = num_bytes + clients[key]._prev_remain;
			char* p = ex_over->_send_buf;
			while (remain_data > 0) {
				int packet_size = p[0];
				if (packet_size <= remain_data) {
					process_packet(static_cast<int>(key), p);
					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else break;
			}
			clients[key]._prev_remain = remain_data;
			if (remain_data > 0) {
				memcpy(ex_over->_send_buf, p, remain_data);
			}
			clients[key].do_recv();
			break;
		}
		case OP_SEND: {
			if (num_bytes != ex_over->_wsabuf.len) {
				disconnect(static_cast<int>(key));
			}
			delete ex_over;
			break;
		}
		}

	}

	closesocket(server_socket);
	CloseHandle(h_iocp);
	WSACleanup();
}