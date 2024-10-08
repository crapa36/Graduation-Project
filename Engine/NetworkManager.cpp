#include "pch.h"
#include "NetworkManager.h"
#include "../Server/demo/protocol.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"

struct OVER_EXP {
    WSAOVERLAPPED _over;
    char _buf[BUF_SIZE];
    WSABUF _wsabuf;
    int _comp_type;
};

SOCKET g_socket;
OVER_EXP g_recv_over;

void NetworkManager::Init(){
    if (!initialize_winsock()) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
    }

    if (!connect_to_server("127.0.0.1")) {
    }

    send_login_packet("PlayerName");
}

void NetworkManager::Update(){
    auto gameObjects = GET_SINGLETON(SceneManager)->GetActiveScene()->GetGameObjects();
    for (auto gameObject : gameObjects) {
        if (gameObject->GetTransform()) {
            auto worldPos = gameObject->GetTransform()->GetWorldPosition();
        }
    }
}

void NetworkManager::LateUpdate()
{
}

void NetworkManager::FinalUpdate()
{
}

bool NetworkManager::initialize_winsock()
{
    WSADATA wsa_data;
    return (WSAStartup(MAKEWORD(2, 2), &wsa_data) == 0);
}

bool NetworkManager::connect_to_server(const char* ip_address)
{
    g_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (g_socket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket. Error: " << WSAGetLastError() << std::endl;
        return false;
    }

    SOCKADDR_IN server_addr;
    ZeroMemory(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT_NUM);
    inet_pton(AF_INET, ip_address, &server_addr.sin_addr);

    if (connect(g_socket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to server. Error: " << WSAGetLastError() << std::endl;
        closesocket(g_socket);
        return false;
    }

    std::cout << "Connected to server successfully." << std::endl;
    return true;
}

void NetworkManager::send_login_packet(const char* name)
{
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

void NetworkManager::cleanup()
{
    closesocket(g_socket);
    WSACleanup();
}

void NetworkManager::send_move_packet(/*호출자로 컴퍼넌트나 오브젝트 받아와서*/) {
    CS_PLAYER_MOVE_PACKET packet;
    //인자로 받은 애의 정보로 변환
    /*packet.Position = Object.GetPosition();
    packet.Rotation = Object.GetRotation();*/
    send(g_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);
}