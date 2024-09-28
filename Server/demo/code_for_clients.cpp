#include "protocol.h"
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iostream>

extern SOCKET g_socket; // Assume this is your global socket connected to the server

//소켓열기
bool initialize_winsock()
{
    WSADATA wsa_data;
    return (WSAStartup(MAKEWORD(2, 2), &wsa_data) == 0);
}

//서버와 연결
bool connect_to_server(const char* ip_address, int port)
{
    g_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (g_socket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket. Error: " << WSAGetLastError() << std::endl;
        return false;
    }

    SOCKADDR_IN server_addr;
    ZeroMemory(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip_address, &server_addr.sin_addr);

    if (connect(g_socket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to server. Error: " << WSAGetLastError() << std::endl;
        closesocket(g_socket);
        return false;
    }

    std::cout << "Connected to server successfully." << std::endl;
    return true;
}

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