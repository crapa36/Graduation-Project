#pragma once
#include <cstdint>

constexpr int PORT_NUM = 4000;
constexpr int BUF_SIZE = 200;
constexpr int MAX_USER = 10;
constexpr int MAX_NAME_SIZE = 20;

struct Vector3_
{
    float x{ 0.f }, y{ 0.f }, z{ 0.f };
    Vector3_() {};
    Vector3_(float a, float b, float c) : x(a), y(b), z(c) {}
};

struct PacketHeader {
    unsigned short size;
    unsigned char type;
};

//ÆÐÅ¶ ID
enum PACKET_TYPE : uint8_t {
    CS_LOGIN = 0,
    SC_LOGIN_OK,
    SC_LOGIN_FAIL,
    SC_LOGIN_INFO,
    SC_ADD_PLAYER,
    CS_PLAYER_MOVE,
    SC_REMOVE_PLAYER,

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

struct SC_LOGIN_INFO_PACKET {
    uint8_t size;
    PACKET_TYPE	type;
    int	id;
    float x;
    float y;
    float z;
    SC_LOGIN_INFO_PACKET() : size(sizeof(SC_LOGIN_INFO_PACKET)), type(PACKET_TYPE::SC_LOGIN_INFO), id(-1), x(0.f), y(0.f), z(0.f) {}
};

struct SC_ADD_PLAYER_PACKET {
    uint8_t size;
    PACKET_TYPE	type;
    int	id;
    float x;
    float y;
    float z;
    char	name[MAX_NAME_SIZE];
    SC_ADD_PLAYER_PACKET() : size(sizeof(SC_ADD_PLAYER_PACKET)), type(PACKET_TYPE::SC_ADD_PLAYER), id(-1), x(0.f), y(0.f), z(0.f), name("\0") {}
};


struct CS_PLAYER_MOVE_PACKET {
    uint8_t size;
    PACKET_TYPE	type;
    int id;
    float x, y, z;
    float rx, ry, rz;

    CS_PLAYER_MOVE_PACKET() : size(sizeof(CS_PLAYER_MOVE_PACKET)), type(CS_PLAYER_MOVE), id(-1), x(0.f), y(0.f), z(0.f), rx(0.f), ry(0.f), rz(0.f) {}

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