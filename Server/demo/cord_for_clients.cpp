#include "protocol.h"
#include <WinSock2.h>
#include <iostream>

void send_login_packet(const char* name);

extern SOCKET g_socket; // Assume this is your global socket connected to the server

//서버로 보내는 로그인 패킷
void send_login_packet(const char* name)
{
    if (strlen(name) >= MAX_NAME_SIZE) {
        std::cerr << "Error: Name is too long. Maximum length is " << MAX_NAME_SIZE - 1 << " characters." << std::endl;
        return;
    }

    CS_LOGIN_PACKET packet;
    strncpy_s(packet.name, name, MAX_NAME_SIZE);
    packet.name[MAX_NAME_SIZE - 1] = '\0'; // Ensure null-termination

    int result = send(g_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "Error sending login packet. Error code: " << WSAGetLastError() << std::endl;
    }
    else {
        std::cout << "Login packet sent successfully." << std::endl;
    }
}