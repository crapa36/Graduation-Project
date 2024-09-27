#include "pch.h"
#include "Input.h"
#include "Engine.h"

bool Input::Init(WindowInfo info) {
    HRESULT result;

    m_screenWidth = info.clientWidth;
    m_screenHeight = info.clientHeight;

    // DirectInput 인터페이스 초기화
    result = DirectInput8Create(info.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, nullptr);
    if (FAILED(result)) {
        OutputDebugStringA("DirectInput8Create failed\n");
        return false;
    }

    // 키보드 장치 초기화
    result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, nullptr);
    if (FAILED(result)) {
        OutputDebugStringA("Failed to create keyboard device\n");
        return false;
    }

    result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
    if (FAILED(result)) {
        OutputDebugStringA("Failed to set keyboard data format\n");
        return false;
    }

    result = m_keyboard->SetCooperativeLevel(info.hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    if (FAILED(result)) {
        OutputDebugStringA("Failed to set keyboard cooperative level\n");
        return false;
    }

    result = m_keyboard->Acquire();
    if (FAILED(result)) {
        OutputDebugStringA("Failed to acquire keyboard\n");
        return false;
    }

    // 마우스 장치 초기화
    result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, nullptr);
    if (FAILED(result)) {
        OutputDebugStringA("Failed to create mouse device\n");
        return false;
    }

    result = m_mouse->SetDataFormat(&c_dfDIMouse);
    if (FAILED(result)) {
        OutputDebugStringA("Failed to set mouse data format\n");
        return false;
    }

    result = m_mouse->SetCooperativeLevel(info.hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    if (FAILED(result)) {
        OutputDebugStringA("Failed to set mouse cooperative level\n");
        return false;
    }

    result = m_mouse->Acquire();
    if (FAILED(result)) {
        OutputDebugStringA("Failed to acquire mouse\n");
        return false;
    }

    // 마우스 시작 위치 설정
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(info.hwnd, &cursorPos);

    m_mousePos = cursorPos;

    return true;
}

void Input::Shutdown() {
    if (m_mouse) {
        m_mouse->Unacquire();
        m_mouse->Release();
        m_mouse = nullptr;
    }

    if (m_keyboard) {
        m_keyboard->Unacquire();
        m_keyboard->Release();
        m_keyboard = nullptr;
    }

    if (m_directInput) {
        m_directInput->Release();
        m_directInput = nullptr;
    }
}

bool Input::Update() {
    memcpy(m_previousKeyboardState, m_keyboardState, sizeof(m_keyboardState));
    memcpy(&m_previousMouseState, &m_mouseState, sizeof(m_mouseState));
    if (!ReadKeyboard() || !ReadMouse()) {
        return false;
    }

    ProcessInput();
    return true;
}

bool Input::ReadKeyboard() {
    HRESULT result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
    if (FAILED(result)) {
        if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) {
            m_keyboard->Acquire();
        }
        else {
            OutputDebugStringA("Failed to get keyboard state\n");
            return false;
        }
    }
    return true;
}

bool Input::ReadMouse() {
    HRESULT result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
    if (FAILED(result)) {
        if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) {
            m_mouse->Acquire();
        }
        else {
            OutputDebugStringA("Failed to get mouse state\n");
            return false;
        }
    }

    // 마우스 절대 위치를 업데이트
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(GEngine->GetWindow().hwnd, &cursorPos);

    m_mousePos = cursorPos;

    // 마우스 휠 업데이트
    m_mouseWheel += m_mouseState.lZ;

    return true;
}

void Input::ProcessInput() {
    // 화면 경계 제한
    if (m_mousePos.x < 0) { m_mousePos.x = 0; }
    if (m_mousePos.y < 0) { m_mousePos.y = 0; }
    if (m_mousePos.x > m_screenWidth) { m_mousePos.x = m_screenWidth; }
    if (m_mousePos.y > m_screenHeight) { m_mousePos.y = m_screenHeight; }
}

bool Input::IsKeyPressed(unsigned char key) {
    return m_keyboardState[key] & 0x80;
}

bool Input::IsKeyJustPressed(unsigned char key) {
    return (m_keyboardState[key] & 0x80) && !(m_previousKeyboardState[key] & 0x80);
}

bool Input::IsKeyJustReleased(unsigned char key) {
    return !(m_keyboardState[key] & 0x80) && (m_previousKeyboardState[key] & 0x80);
}

bool Input::IsMouseButtonPressed(int button) {
    return (m_mouseState.rgbButtons[button] & 0x80) != 0;
}

bool Input::IsMouseButtonJustPressed(int button) {
    return (m_mouseState.rgbButtons[button] & 0x80) && !(m_previousMouseState.rgbButtons[button] & 0x80);
}

bool Input::IsMouseButtonJustReleased(int button) {
    return !(m_mouseState.rgbButtons[button] & 0x80) && (m_previousMouseState.rgbButtons[button] & 0x80);
}
