#include "pch.h"
#include "Input.h"
#include "Engine.h"

void Input::Init(HWND hwnd) {
    _hwnd = hwnd;
    _states.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);
}

void Input::Update() {
    // ���� Ȱ��ȭ�� â�� �ٸ� ��� Ű ���� �ʱ�ȭ
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
            // Ű�� ���� ���¸� PRESS �Ǵ� DOWN���� ����
            state = (state == KEY_STATE::NONE || state == KEY_STATE::UP) ? KEY_STATE::DOWN : KEY_STATE::PRESS;
        }
        else {
            // Ű�� �������� ������ UP �Ǵ� NONE���� ����
            state = (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN) ? KEY_STATE::UP : KEY_STATE::NONE;
        }
    }

    ::GetCursorPos(&_mousePos);
    ::ScreenToClient(_hwnd, &_mousePos);
}
