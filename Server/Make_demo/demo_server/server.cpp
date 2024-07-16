#include <iostream>
#include <unordered_map>
#include <mutex>
#include <WS2tcpip.h>

#pragma comment (lib, "WS2_32.LIB")

constexpr short PORT = 4000;
constexpr int BUFSIZE = 256;

bool b_shutdwn = false;

class SESSION;

std::unordered_map<int, SESSION> g_players;
std::unordered_map<LPWSAOVERLAPPED, int> g_session_map;
std::mutex g_players_mutex;

void CALLBACK send_callback(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK recv_callback(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void print_error(const char* msg, int err_no);

//세션 클래스
class SESSION {
	char buf[BUFSIZE];	//데이터 저장
	WSABUF wsabuf[1];	//WSA버퍼 구조체
	SOCKET client_s;	//클라 소켓
	WSAOVERLAPPED over;	//비동기 I/O작업을 위한 OVERLAPPED구조체
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
	~SESSION() { 
		closesocket(client_s); 
	}
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

	void do_send(int s_id, char* mess, int recv_size) {

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
	WCHAR* msg_buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&msg_buf), 0, NULL);
	std::cout << msg;
	std::wcout << L" : 에러 : " << msg_buf << std::endl;
	while (true);
	LocalFree(msg_buf);
}

void CALLBACK recv_callback(DWORD err, DWORD recv_size,	LPWSAOVERLAPPED pover, DWORD recv_flag)
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


int main() {
	std::wcout.imbue(std::locale("korean"));

	WSADATA WSAData;
	//ws2_32.dll 초기화 및 소켓 API 사용 가능하도록
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
		std::cout << "WSAStartup이 정상적으로 호출 되지 않았습니다" << std::endl;
	}
	//소켓 및 서버 주소 설정
	SOCKET server_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	bind(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(server_socket, SOMAXCONN);

	//호스트 설정
	char hostname[256];
	if (gethostname(hostname, sizeof(hostname)) == 0) {
		struct addrinfo hints, * res;
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;

		if (getaddrinfo(hostname, NULL, &hints, &res) == 0) {
			char ip_address[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &((struct sockaddr_in*)res->ai_addr)->sin_addr, ip_address, INET_ADDRSTRLEN);
			std::cout << "서버 IP 주소: " << ip_address << std::endl;
			freeaddrinfo(res);
		}
	}
	std::cout << "서버가 시작되었습니다. 포트: " << PORT << std::endl;

	int addr_size = sizeof(server_addr);
	int id = 0;

	while (false == b_shutdwn)
	{
		SOCKET client_socket = WSAAccept(server_socket, reinterpret_cast<sockaddr*>(&server_addr), &addr_size, nullptr, 0);
		if (client_socket == INVALID_SOCKET) {
			std::cout << "클라이언트 연결 수락 실패. 에러 코드: " << WSAGetLastError() << std::endl;
			continue;
		}

		char client_ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(server_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
		std::cout << "새로운 클라이언트 접속: ID " << id << ", IP " << client_ip << ", 포트 " << ntohs(server_addr.sin_port) << std::endl;

		g_players.try_emplace(id, client_socket, id);
		g_players[id++].do_recv();
	}

	g_players.clear();
	closesocket(server_socket);	//소켓닫기		
	WSACleanup();				//종료시 호출
}