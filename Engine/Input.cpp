#include "pch.h"
#include "Input.h"
#include "Engine.h"

void Input::Init(HWND hwnd) {
    _hwnd = hwnd;
    _states.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);
}

void Input::Update() {
    // 현재 활성화된 창이 다른 경우 키 상태 초기화
    if (_hwnd != ::GetActiveWindow()) {
        memset(_states.data(), static_cast<int>(KEY_STATE::NONE), _states.size() * sizeof(KEY_STATE));
        return;
    }

    BYTE asciiKeys[KEY_TYPE_COUNT] = {};
    if (::GetKeyboardState(asciiKeys) == FALSE) {
        return;
    }

    for (uint32 key = 0; key < KEY_TYPE_COUNT; ++key) {
        KEY_STATE& state = _states[key];
        bool isKeyDown = asciiKeys[key] & 0x80;

        if (isKeyDown) {
            // 키가 눌린 상태면 PRESS 또는 DOWN으로 설정
            state = (state == KEY_STATE::NONE || state == KEY_STATE::UP) ? KEY_STATE::DOWN : KEY_STATE::PRESS;
        }
        else {
            // 키가 눌려있지 않으면 UP 또는 NONE으로 설정
            state = (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN) ? KEY_STATE::UP : KEY_STATE::NONE;
        }
    }

    ::GetCursorPos(&_mousePos);
    ::ScreenToClient(_hwnd, &_mousePos);
}
