#include <iostream>
#include <WS2tcpip.h>
#include <thread>
#include <mutex>
#include <string>
#pragma comment (lib, "WS2_32.LIB")

constexpr short PORT = 4000;
constexpr char SERVER_ADDR[] = "172.30.1.5";
constexpr int BUFSIZE = 256;

bool bshutdown = false;
SOCKET server_s;
WSABUF wsabuf[1];
char buf[BUFSIZE];
WSAOVERLAPPED wsaover;
std::mutex cout_mutex;

void CALLBACK send_callback(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK recv_callback(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);

void print_message(const char* message, int size) {
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << "\r";  // 현재 줄의 시작으로 이동
    std::cout << "Received: ";
    std::cout.write(message, size);
    std::cout << std::endl;
    std::cout << "Enter Message: " << std::flush;
}

void send_message(const std::string& message) {
    strcpy_s(buf, message.c_str());
    wsabuf[0].len = static_cast<ULONG>(message.length() + 1);
    ZeroMemory(&wsaover, sizeof(wsaover));
    WSASend(server_s, wsabuf, 1, nullptr, 0, &wsaover, send_callback);
}

void receive_messages() {
    while (!bshutdown) {
        DWORD recv_flag = 0;
        ZeroMemory(&wsaover, sizeof(wsaover));
        if (WSARecv(server_s, wsabuf, 1, nullptr, &recv_flag, &wsaover, recv_callback) == SOCKET_ERROR) {
            int error = WSAGetLastError();
            if (error != WSA_IO_PENDING) {
                std::cout << "WSARecv failed with error: " << error << std::endl;
                bshutdown = true;
                return;
            }
        }
        SleepEx(100, TRUE);
    }
}

void CALLBACK recv_callback(DWORD err, DWORD recv_size,
    LPWSAOVERLAPPED pwsaover, DWORD sendflag)
{
    if (err != 0 || recv_size == 0) {
        bshutdown = true;
        return;
    }

    int p_size = 0;
    while (recv_size > p_size) {
        int m_size = static_cast<unsigned char>(buf[p_size]);
        if (m_size <= 2 || p_size + m_size > recv_size) break;  // 잘못된 메시지 형식 체크

        int sender_id = static_cast<unsigned char>(buf[p_size + 1]);
        std::string message(buf + p_size + 2, m_size - 2);

        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "\rPlayer [" << sender_id << "]: " << message << std::endl;
        std::cout << "Enter Message: " << std::flush;

        p_size += m_size;
    }

    // 다음 수신을 위해 버퍼 재설정
    wsabuf[0].len = BUFSIZE;
}

void CALLBACK send_callback(DWORD err, DWORD sent_size,
    LPWSAOVERLAPPED pwsaover, DWORD sendflag)
{
    if (err != 0) {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Send error: " << err << std::endl;
    }
}

int main()
{
    std::wcout.imbue(std::locale("korean"));
    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 0), &WSAData);
    server_s = WSASocket(AF_INET, SOCK_STREAM,
        IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);

    SOCKADDR_IN server_a;
    server_a.sin_family = AF_INET;
    server_a.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_ADDR, &server_a.sin_addr);

    if (connect(server_s, reinterpret_cast<sockaddr*>(&server_a), sizeof(server_a)) == SOCKET_ERROR) {
        std::cout << "Connection failed. Error: " << WSAGetLastError() << std::endl;
        closesocket(server_s);
        WSACleanup();
        return 1;
    }

    wsabuf[0].buf = buf;
    wsabuf[0].len = BUFSIZE;

    std::thread recv_thread(receive_messages);

    std::string input;
    while (!bshutdown) {
        std::cout << "Enter Message: " << std::flush;
        std::getline(std::cin, input);
        if (input.empty()) {
            bshutdown = true;
        }
        else {
            send_message(input);
        }
    }

    recv_thread.join();
    closesocket(server_s);
    WSACleanup();
    return 0;
}