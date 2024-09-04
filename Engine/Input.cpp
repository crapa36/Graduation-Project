#include "pch.h"
#include "Input.h"
#include "Engine.h"
#include <array>
#include <algorithm>

void Input::Init(HWND hwnd) {
    _hwnd = hwnd;
    _states.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);
}

void Input::Update() {
    // 현재 활성화된 창이 다른 경우 키 상태 초기화
    if (_hwnd != ::GetActiveWindow()) {
        std::fill(_states.begin(), _states.end(), KEY_STATE::NONE);
        return;
    }

    std::array<BYTE, 256> asciiKeys = {};
    if (!::GetKeyboardState(asciiKeys.data())) {
        return;
    }

    for (uint32 key = 0; key < KEY_TYPE_COUNT; ++key) {
        bool isKeyDown = asciiKeys[static_cast<int>(key)] & 0x80;

        _states[key] = [isKeyDown, state = _states[key]]() mutable {
            switch (state) {
            case KEY_STATE::NONE:   return isKeyDown ? KEY_STATE::DOWN : KEY_STATE::NONE;
            case KEY_STATE::DOWN:   return isKeyDown ? KEY_STATE::PRESS : KEY_STATE::UP;
            case KEY_STATE::PRESS:  return isKeyDown ? KEY_STATE::PRESS : KEY_STATE::UP;
            case KEY_STATE::UP:     return KEY_STATE::NONE;
            }
            return KEY_STATE::NONE;
            }();
    }

    ::GetCursorPos(&_mousePos);
    ::ScreenToClient(_hwnd, &_mousePos);
}
