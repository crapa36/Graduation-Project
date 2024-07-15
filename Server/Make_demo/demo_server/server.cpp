#include <iostream>
#include <unordered_map>
#include <WS2tcpip.h>

#pragma comment (lib, "WS2_32.LIB")

constexpr short PORT = 4000;
bool b_shutdwn = false;

class SESSION;

std::unordered_map<int, SESSION> g_players;

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
		std::cout << "새로운 클라이언트 접속: ID " << id << ", IP " << client_ip << ", 포트 " << ntohs(server_a.sin_port) << std::endl;

		g_players.try_emplace(id, client_socket, id);
		g_players[id++].do_recv();
	}

	g_players.clear();
	closesocket(server_socket);	//소켓닫기		
	WSACleanup();				//종료시 호출
}