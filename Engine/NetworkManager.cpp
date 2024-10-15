#include "pch.h"
#include "NetworkManager.h"
#include "../Server/demo/protocol.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"
#include "MeshData.h"
#include "MeshRenderer.h"
#include "Resources.h"
#include "BoxCollider.h"
#include "Rigidbody.h"


struct PacketBuffer {
    char buffer[BUF_SIZE];
    int dataSize;
};

SOCKET g_socket;
OVER_EXP g_recv_over;
int client_id;
PacketBuffer g_packetBuffer;


void NetworkManager::Init(){
    if (!initialize_winsock()) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
    }
    if (!connect_to_server("127.0.0.1")) {
    }
    
    g_recv_over._wsabuf.len = BUF_SIZE;
    g_recv_over._wsabuf.buf = g_recv_over._send_buf;
    DWORD flags = 0;
    WSARecv(g_socket, &g_recv_over._wsabuf, 1, NULL, &flags, &g_recv_over._over, RecvCallback);
    send_login_packet();
}



void NetworkManager::Update(){
    auto gameObjects = GET_SINGLETON(SceneManager)->GetActiveScene()->GetGameObjects();
    auto now = std::chrono::steady_clock::now();

    for (auto gameObject : gameObjects) {
        if (gameObject->GetName() == L"Main") {
            if (gameObject->GetTransform()) {
                auto worldPos = gameObject->GetTransform()->GetWorldPosition();
                auto worldRot = gameObject->GetTransform()->GetLook();

                if (Vector3::Distance(worldPos, lastSentPosition) > positionThreshold ||
                    std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSendTime).count() > 100) {

                    //방향 가져오기
                    send_move_packet(worldPos, worldRot);
                    lastSentPosition = worldPos;
                    lastSendTime = now;
                }
            }
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

void NetworkManager::send_login_packet()
{
    CS_LOGIN_PACKET packet;
    
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
    if (dataLength < sizeof(PacketHeader)) {
        std::cerr << "Invalid packet size" << std::endl;
        return;
    }

    
    switch (packet[1]) {  // packet[1]은 패킷 타입을 나타낸다고 가정
    case SC_LOGIN_INFO: {
        SC_LOGIN_INFO_PACKET* p = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(packet);
        std::cout << "Login confirmed. Assigned ID: " << p->id << std::endl;
        client_id = p->id;
        break;
    }
    case SC_ADD_PLAYER: {
        SC_ADD_PLAYER_PACKET* p = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(packet);
        //std::cout << "New player added. ID: " << p->id << ", Name: " << p->name << std::endl;
        {
            wstring path = L"../Resources/BIN/Gunship.bin";
            shared_ptr<MeshData> meshData = GET_SINGLETON(Resources)->LoadBIN(path);

            shared_ptr<GameObject> client = make_shared<GameObject>();

            
            client->SetName(L"Main");
            client->SetCheckFrustum(false);
            //id? 

            client->AddComponent(make_shared<Transform>());
            shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
            {
                shared_ptr<Mesh> sphereMesh = GET_SINGLETON(Resources)->LoadPointMesh();
                meshRenderer->SetMesh(sphereMesh);
            }
            {
                shared_ptr<Material> material = GET_SINGLETON(Resources)->Get<Material>(L"Pebbles");
                meshRenderer->SetMaterial(material->Clone());
            }
            client->AddComponent(meshRenderer);

            client->AddComponent(make_shared<BoxCollider>());
            client->AddComponent(make_shared<Rigidbody>());
            dynamic_pointer_cast<BoxCollider>(client->GetCollider())->SetExtents(Vec3(40.f, 30.f, 40.f));
            dynamic_pointer_cast<BoxCollider>(client->GetCollider())->SetCenter(Vec3(0.f, 10.f, 0.f));
            client->GetTransform()->SetLocalPosition(Vec3(p->x, p->y, p->z)); //여기가 좌표
            client->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f)); // 여기가 크기
            client->GetRigidbody()->SetUseGravity(false);
            client->GetRigidbody()->SetElasticity(0.0f);
            client->GetRigidbody()->SetDrag(0.95f);

            client->SetStatic(false);
            GET_SINGLETON(SceneManager)->GetActiveScene()->AddGameObject(client);
           

            vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

            for (auto& gameObject : gameObjects) {
                gameObject->SetCheckFrustum(false);

                gameObject->SetParent(client);

                gameObject->SetStatic(false);
                GET_SINGLETON(SceneManager)->GetActiveScene()->AddGameObject(gameObject);
            }
        }
        break;
    }
    case SC_REMOVE_PLAYER: {
        SC_REMOVE_PLAYER_PACKET* p = reinterpret_cast<SC_REMOVE_PLAYER_PACKET*>(packet);
        std::cout << "Player removed. ID: " << p->id << std::endl;
        // 여기서 해당 ID를 가진 플레이어 오브젝트를 찾아 제거
        break;
    }
    case CS_PLAYER_MOVE: {
        CS_PLAYER_MOVE_PACKET* p = reinterpret_cast<CS_PLAYER_MOVE_PACKET*>(packet);
         
         //GameObject* player = FindPlayerById(p->id);
         //if (player)
             //player->GetTransform()->SetPosition(Vec3(p->x, p->y, p->z));
        break;
    }
    default:
        int num = static_cast<int>(packet[0]);
        std::cout << "Unknown packet type received: " << num << std::endl;
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

void NetworkManager::send_move_packet(Vec3 Pos, Vec3 Rot) {
    CS_PLAYER_MOVE_PACKET packet;
    packet.id = client_id;
    packet.x = Pos.x;
    packet.y = Pos.y;
    packet.z = Pos.z;
    packet.rx = Rot.x;
    packet.ry = Rot.y;
    packet.rz = Rot.z;
    send(g_socket, reinterpret_cast<char*>(&packet), sizeof(packet), 0);
}