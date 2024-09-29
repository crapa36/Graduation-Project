#pragma once
#pragma comment(lib, "ws2_32.lib")

extern SOCKET g_socket;

bool initialize_winsock();
bool connect_to_server(const char* ip_address);
void send_login_packet(const char* name);
void cleanup();
