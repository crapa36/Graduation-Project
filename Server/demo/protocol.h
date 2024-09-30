#pragma once
#include "../../Engine/pch.h"
#include <cstdint>

constexpr int PORT_NUM = 4000;
constexpr int BUF_SIZE = 200;
constexpr int MAX_USER = 10;
constexpr int MAX_NAME_SIZE = 20;



//ÆÐÅ¶ ID
enum PACKET_TYPE : uint8_t {
    CS_LOGIN = 0,
    SC_LOGIN_OK,
    SC_LOGIN_FAIL,
    SC_LOGIN_INFO,
    SC_ADD_PLAYER,
    CS_PLAYER_MOVE,
    SC_REMOVE_PLAYER = 5,

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
    Vec3 Position;
    SC_LOGIN_INFO_PACKET() : size(sizeof(SC_LOGIN_INFO_PACKET)), type(PACKET_TYPE::SC_LOGIN_INFO), id(-1), Position(0.0f, 0.0f, 0.0f) {}
};

struct SC_ADD_PLAYER_PACKET {
    uint8_t size;
    PACKET_TYPE	type;
    int	id;
    Vec3 Position;
    char	name[MAX_NAME_SIZE];
    SC_ADD_PLAYER_PACKET() : size(sizeof(SC_ADD_PLAYER_PACKET)), type(PACKET_TYPE::SC_ADD_PLAYER), id(-1), Position(0.0f, 0.0f, 0.0f) {}
};


struct CS_PLAYER_MOVE_PACKET {
    uint8_t size;
    PACKET_TYPE	type;
    int id;
    Vec3 Position;
    Vec3 Rotation;

    CS_PLAYER_MOVE_PACKET() : size(sizeof(CS_PLAYER_MOVE_PACKET)), type(CS_PLAYER_MOVE), id(-1), Position(0.0f, 0.0f, 0.0f), Rotation(0.0f, 0.0f, 0.0f) {}

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