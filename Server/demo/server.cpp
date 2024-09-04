#include <iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>

int main() {
	WSAData WSADATA;
	WSAStartup(MAKEWORD(2, 2), &WSADATA);


	WSACleanup();
}