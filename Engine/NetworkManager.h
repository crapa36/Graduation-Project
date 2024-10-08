#pragma once
#pragma comment(lib, "ws2_32.lib")

extern SOCKET g_socket;



class NetworkManager {
    DECLARE_SINGLETON(NetworkManager);

public:

    void Init();
    void Update();
    void LateUpdate();
    void FinalUpdate();
private:
    bool initialize_winsock();
    bool connect_to_server(const char* ip_address);
    void send_login_packet(const char* name);
    void cleanup();

    void send_move_packet(Vec3 Pos);
   
};