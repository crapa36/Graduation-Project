#include "pch.h"
#include "NetworkManager.h"
#include "../Server/demo/protocol.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"

enum COMP_TYPE { OP_ACCEPT, OP_RECV, OP_SEND };
class OVER_EXP {
public:
    WSAOVERLAPPED _over;
    WSABUF _wsabuf;
    char _send_buf[BUF_SIZE];
    COMP_TYPE _comp_type;
    OVER_EXP()
    {
        _wsabuf.len = BUF_SIZE;
        _wsabuf.buf = _send_buf;
        _comp_type = OP_RECV;
        ZeroMemory(&_over, sizeof(_over));
    }
    OVER_EXP(char* packet)
    {
        _wsabuf.len = packet[0];
        _wsabuf.buf = _send_buf;
        ZeroMemory(&_over, sizeof(_over));
        _comp_type = OP_SEND;
        memcpy(_send_buf, packet, packet[0]);
    }
};

SOCKET g_socket;
OVER_EXP g_recv_over;
int client_id;

void NetworkManager::Init(){
    if (!initialize_winsock()) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
    }
    if (!connect_to_server("127.0.0.1")) {
    }
    send_login_packet("PlayerName");
    g_recv_over._wsabuf.len = BUF_SIZE;
    g_recv_over._wsabuf.buf = g_recv_over._send_buf;
    DWORD flags = 0;
    WSARecv(g_socket, &g_recv_over._wsabuf, 1, NULL, &flags, &g_recv_over._over, RecvCallback);
}

void NetworkManager::Update(){
    auto gameObjects = GET_SINGLETON(SceneManager)->GetActiveScene()->GetGameObjects();
    for (auto gameObject : gameObjects) {
        if (gameObject->GetName() == L"Main") {
            if (gameObject->GetTransform()) {
                auto worldPos = gameObject->GetTransform()->GetWorldPosition();
                //방향 가져오기
                send_move_packet(worldPos);
            }
        }
    }
    DWORD flags = 0;
    WSARecv(g_socket, &g_recv_over._wsabuf, 1, NULL, &flags, &g_recv_over._over, RecvCallback);

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

void ProcessPacket(char* packet, DWORD dataLength)
{
    switch (packet[1]) {  // packet[1]은 패킷 타입을 나타낸다고 가정
    case SC_LOGIN_INFO: {
        SC_LOGIN_INFO_PACKET* p = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(packet);
        std::cout << "Login confirmed. Assigned ID: " << p->id << std::endl;
        client_id = p->id;
        break;
    }
    case SC_ADD_PLAYER: {
        SC_ADD_PLAYER_PACKET* p = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(packet);
        std::cout << "New player added. ID: " << p->id << ", Name: " << p->name << std::endl;
        // 여기서 새로운 플레이어 오브젝트를 생성하고 씬에 추가
        // GameObject* newPlayer = CREATE_OBJECT(Player);
        // newPlayer->GetTransform()->SetPosition(p->Position);
        break;
    }
    case SC_REMOVE_PLAYER: {
        SC_REMOVE_PLAYER_PACKET* p = reinterpret_cast<SC_REMOVE_PLAYER_PACKET*>(packet);
        std::cout << "Player removed. ID: " << p->id << std::endl;
        // 여기서 해당 ID를 가진 플레이어 오브젝트를 찾아 제거
        break;
    }
    //case SC_PLAYER_MOVE: {
        //SC_PLAYER_MOVE_PACKET* p = reinterpret_cast<SC_PLAYER_MOVE_PACKET*>(packet);
        // 다른 플레이어의 위치 업데이트
        // GameObject* player = FindPlayerById(p->id);
        // if (player)
        //     player->GetTransform()->SetPosition(Vec3(p->x, p->y, p->z));
        //break;
    //}
    default:
        std::cout << "Unknown packet type received: " << (int)packet[1] << std::endl;
        break;
    }
}

void CALLBACK RecvCallback(DWORD error, DWORD dataLength, LPWSAOVERLAPPED overlapped, DWORD flags) {
    if (error || dataLength == 0) {
        // 연결 종료 처리
        return;
    }

    // 패킷 처리
    ProcessPacket(g_recv_over._send_buf, dataLength);

    // 다음 수신 대기
    flags = 0;
    WSARecv(g_socket, &g_recv_over._wsabuf, 1, NULL, &flags, &g_recv_over._over, RecvCallback);
}

void NetworkManager::cleanup()
{
    closesocket(g_socket);
    WSACleanup();
}

void NetworkManager::send_move_packet(Vec3 Pos) {
    CS_PLAYER_MOVE_PACKET packet;
    packet.id = client_id;
    packet.x = Pos.x;
    packet.y = Pos.y;
    packet.z = Pos.z;
    //packet.Rotation = Object.GetRotation();
    send(g_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);
}