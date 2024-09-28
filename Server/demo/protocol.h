#pragma once
#include <cstdint>

constexpr int PORT_NUM = 4000;
constexpr int BUF_SIZE = 200;
constexpr int MAX_USER = 10;
constexpr int MAX_NAME_SIZE = 20;



//ÆÐÅ¶ ID
enum PACKET_TYPE : uint8_t {
    CS_LOGIN = 1,
    SC_LOGIN_OK,
    SC_LOGIN_FAIL,
    SC_REMOVE_PLAYER = 4,

    // ... other packet types ...
};

#pragma pack(push, 1)
struct CS_LOGIN_PACKET {
    uint8_t size;
    PACKET_TYPE type;
    char name[MAX_NAME_SIZE];

    CS_LOGIN_PACKET() : size(sizeof(CS_LOGIN_PACKET)), type(CS_LOGIN) {
        name[0] = '\0';
    }
};

struct SC_LOGIN_OK_PACKET {
    uint8_t size;
    PACKET_TYPE type;
    int id;

    SC_LOGIN_OK_PACKET() : size(sizeof(SC_LOGIN_OK_PACKET)), type(SC_LOGIN_OK), id(-1) {}
};

struct SC_LOGIN_FAIL_PACKET {
    uint8_t size;
    PACKET_TYPE type;
    char reason[50];

    SC_LOGIN_FAIL_PACKET() : size(sizeof(SC_LOGIN_FAIL_PACKET)), type(SC_LOGIN_FAIL) {
        reason[0] = '\0';
    }
};


struct SC_REMOVE_PLAYER_PACKET {
    uint8_t size;
    PACKET_TYPE type;
    int id;
    SC_REMOVE_PLAYER_PACKET() : size(sizeof(SC_REMOVE_PLAYER_PACKET)), type(SC_REMOVE_PLAYER), id(-1) {}
};

#pragma pack(pop)

// Client-side function declaration
void send_login_packet(const char* name);