#pragma once
#include "GameObject.h"
#pragma comment(lib, "ws2_32.lib")

extern SOCKET g_socket;
extern int client_id;



void CALLBACK RecvCallback(DWORD error, DWORD dataLength, LPWSAOVERLAPPED overlapped, DWORD flags);
void ProcessPacket(char* packet, DWORD dataLength);
class NetworkManager {
    DECLARE_SINGLETON(NetworkManager);

public:

    void Init();
    void Update();
    void LateUpdate();
    void FinalUpdate();

private:
    Vec3 lastSentPosition;                              //�ֱ� ��ġ
    float positionThreshold = 0.1f;                     // ��ġ ���� �Ӱ谪
    std::chrono::steady_clock::time_point lastSendTime; //�ֱ� ��ġ�����ð�

    bool initialize_winsock();
    bool connect_to_server(const char* ip_address);
    
    void cleanup();
    void send_login_packet();
    void send_move_packet(Vec3 Pos, Vec3 Rot);
   
};