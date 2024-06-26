#include <iostream>
#include <WS2tcpip.h>
#include <unordered_map>
#include <mutex>
#pragma comment (lib, "WS2_32.LIB")

constexpr short PORT = 4000;
constexpr int BUFSIZE = 256;

bool b_shutdown = false;

class SESSION;

std::unordered_map<LPWSAOVERLAPPED, int> g_session_map;
std::unordered_map<int, SESSION> g_players;
std::mutex g_players_mutex;

void CALLBACK send_callback(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK recv_callback(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void print_error(const char* msg, int err_no);

class EXP_OVER
{
public:
	WSAOVERLAPPED over;
	WSABUF wsabuf[1];
	char buf[BUFSIZE];
	EXP_OVER(int s_id, char* mess, int m_size)
	{
		ZeroMemory(&over, sizeof(over));
		wsabuf[0].buf = buf;
		wsabuf[0].len = m_size + 2;

		buf[0] = m_size + 2;
		buf[1] = s_id;
		memcpy(buf + 2, mess, m_size);
	}
};

class SESSION {
	char buf[BUFSIZE];
	WSABUF wsabuf[1];
	SOCKET client_s;
	WSAOVERLAPPED over;
public:
	SESSION(SOCKET s, int my_id) : client_s(s) {
		g_session_map[&over] = my_id;
		wsabuf[0].buf = buf;
		wsabuf[0].len = BUFSIZE;
	}
	SESSION() {
		std::cout << "ERROR";
		exit(-1);
	}
	~SESSION() { closesocket(client_s); }
	void do_recv()
	{
		DWORD recv_flag = 0;
		ZeroMemory(&over, sizeof(over));
		int res = WSARecv(client_s, wsabuf, 1, nullptr, &recv_flag, &over, recv_callback);
		if (0 != res) {
			int err_no = WSAGetLastError();
			if (WSA_IO_PENDING != err_no)
				print_error("WSARecv", WSAGetLastError());
		}
	}

	void do_send(int s_id, char *mess, int recv_size)
	{
		auto b = new EXP_OVER(s_id, mess, recv_size);
		int res = WSASend(client_s, b->wsabuf, 1, nullptr, 0, &b->over, send_callback);
		if (0 != res) {
			print_error("WSARecv", WSAGetLastError());
		}
	}

	void print_message(DWORD recv_size)
	{
		int my_id = g_session_map[&over];
		std::cout << "Client[" << my_id << "] Sent : ";
		for (DWORD i = 0; i < recv_size; ++i)
			std::cout << buf[i];
		std::cout << std::endl;
	}

	void broadcast(int m_size)
	{
		for (auto& p : g_players)
			p.second.do_send(g_session_map[&over], buf, m_size);
	}
};

void print_error(const char* msg, int err_no)
{
	WCHAR * msg_buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&msg_buf), 0, NULL);
	std::cout << msg;
	std::wcout << L" : 에러 : " << msg_buf << std::endl;
	while (true);
	LocalFree(msg_buf);
}

void CALLBACK send_callback(DWORD err, DWORD sent_size,
	LPWSAOVERLAPPED pover, DWORD recv_flag)
{
	if (0 != err) {
		print_error("WSASend", WSAGetLastError());
	}
	auto b = reinterpret_cast<EXP_OVER*>(pover);
	delete b;
}

void CALLBACK recv_callback(DWORD err, DWORD recv_size, 
	LPWSAOVERLAPPED pover, DWORD recv_flag)
{
	if (0 != err) {
		print_error("WSARecv", WSAGetLastError());
		return;
	}
	int my_id = g_session_map[pover];
	if (0 == recv_size) {
		std::cout << "클라이언트 연결 종료: ID " << my_id << std::endl;
		std::lock_guard<std::mutex> lock(g_players_mutex);
		g_players.erase(my_id);
		return;
	}
	g_players[my_id].print_message(recv_size);
	g_players[my_id].broadcast(recv_size);
	g_players[my_id].do_recv();
}

int main()
{
	std::wcout.imbue(std::locale("korean"));

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	SOCKET server_s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_a;
	server_a.sin_family = AF_INET;
	server_a.sin_port = htons(PORT);
	server_a.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	bind(server_s, reinterpret_cast<sockaddr*>(&server_a), sizeof(server_a));
	listen(server_s, SOMAXCONN);

	char host_name[256];
	if (gethostname(host_name, sizeof(host_name)) == 0) {
		struct addrinfo hints, * res;
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;

		if (getaddrinfo(host_name, NULL, &hints, &res) == 0) {
			char ip_address[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &((struct sockaddr_in*)res->ai_addr)->sin_addr, ip_address, INET_ADDRSTRLEN);
			std::cout << "서버 IP 주소: " << ip_address << std::endl;
			freeaddrinfo(res);
		}
	}
	std::cout << "서버가 시작되었습니다. 포트: " << PORT << std::endl;

	int addr_size = sizeof(server_a);
	int id = 0;
	while (false == b_shutdown) {
		SOCKET client_s = WSAAccept(server_s, reinterpret_cast<sockaddr*>(&server_a), &addr_size, nullptr, 0);
		if (client_s == INVALID_SOCKET) {
			std::cout << "클라이언트 연결 수락 실패. 에러 코드: " << WSAGetLastError() << std::endl;
			continue;
		}

		char client_ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(server_a.sin_addr), client_ip, INET_ADDRSTRLEN);
		std::cout << "새로운 클라이언트 접속: ID " << id << ", IP " << client_ip << ", 포트 " << ntohs(server_a.sin_port) << std::endl;

		g_players.try_emplace(id, client_s, id);
		g_players[id++].do_recv();
	}
	g_players.clear();
	closesocket(server_s);
	WSACleanup();
}