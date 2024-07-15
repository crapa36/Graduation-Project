#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "WS2_32.LIB")


int main() {
	std::wcout.imbue(std::locale("korean"));

	WSADATA WSAData;
	//ws2_32.dll 초기화 및 소켓 API 사용 가능하도록
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
		std::cout << "WSAStartup이 정상적으로 호출 되지 않았습니다" << std::endl;
	}

	WSACleanup();				//종료시 호출
}