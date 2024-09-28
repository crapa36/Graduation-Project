#include <iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <array>
#include <vector>

//#include "../../Engine/EnginePch.h"
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
	float x;
	float y;
	float z;

	SESSION()
	{
		_id = -1;
		_socket = 0;
		x = 0.0;
		y = 0.0;
		z = 0.0;
		_name[0] = 0;
		_state = ST_FREE;
		_prev_remain = 0;
	}

	~SESSION() {}

	void do_send(void* packet)
	{
		OVER_EXP* sdata = new OVER_EXP{ reinterpret_cast<char*>(packet) };
		WSASend(_socket, &sdata->_wsabuf, 1, 0, 0, &sdata->_over, 0);
	}

	void send_remove_player_packet(int c_id)
	{
		SC_REMOVE_PLAYER_PACKET p;
		p.id = c_id;
		p.size = sizeof(p);
		p.type = SC_REMOVE_PLAYER;
		do_send(&p);
	}
};

std::array<SESSION, MAX_USER> clients;

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
		std::cout << "Client [" << c_id << "] Connected\n";
	}
	closesocket(clients[c_id]._socket);
	clients[c_id]._state = ST_FREE;
}

void process_accept(SOCKET client_socket, SOCKADDR_IN& addr, HANDLE h_iocp)
{
	int new_id = get_new_client_id();
	if (new_id == -1) {
		std::cout << "Client Full\n";
		closesocket(client_socket);
		return;
	}

	SESSION& new_client = clients[new_id];
	new_client._id = new_id;
	new_client._socket = client_socket;
	new_client._state = ST_INGAME;

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_socket), h_iocp, new_id, 0);

	std::cout << "Client [" << new_id << "] Connected\n";

	DWORD flags = 0;
	WSARecv(client_socket, &new_client._recv_over._wsabuf, 1, NULL, &flags, &new_client._recv_over._over, NULL);
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
	SOCKET client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	OVER_EXP a_over;
	a_over._comp_type = OP_ACCEPT;
	AcceptEx(server_socket, client_socket, a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &a_over._over);

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
		case OP_ACCEPT:
			process_accept(client_socket, cl_addr, h_iocp);
			client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			ZeroMemory(&a_over._over, sizeof(a_over._over));
			AcceptEx(server_socket, client_socket, a_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &a_over._over);
			break;
		case OP_RECV:
			// Handle receive operation
			break;
		case OP_SEND:
			if (num_bytes != ex_over->_wsabuf.len) {
				disconnect(static_cast<int>(key));
			}
			delete ex_over;
			break;
		}

	}



	closesocket(server_socket);
	CloseHandle(h_iocp);
	WSACleanup();
}