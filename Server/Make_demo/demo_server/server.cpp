#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "WS2_32.LIB")


int main() {
	std::wcout.imbue(std::locale("korean"));

	WSADATA WSAData;
	//ws2_32.dll �ʱ�ȭ �� ���� API ��� �����ϵ���
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
		std::cout << "WSAStartup�� ���������� ȣ�� ���� �ʾҽ��ϴ�" << std::endl;
	}

	WSACleanup();				//����� ȣ��
}