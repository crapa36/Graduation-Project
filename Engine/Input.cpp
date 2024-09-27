#include "pch.h"
#include "Input.h"
#include "Engine.h"

bool Input::Init(WindowInfo info) {
    HRESULT result;

    _screenWidth = info.clientWidth;
    _screenHeight = info.clientHeight;

    // DirectInput 인터페이스 초기화
    result = DirectInput8Create(info.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&_directInput, nullptr);
    if (FAILED(result)) {
        OutputDebugStringA("DirectInput8Create failed\n");
        return false;
    }

    // 키보드 장치 초기화
    result = _directInput->CreateDevice(GUID_SysKeyboard, &_keyboard, nullptr);
    if (FAILED(result)) {
        OutputDebugStringA("Failed to create keyboard device\n");
        return false;
    }

    result = _keyboard->SetDataFormat(&c_dfDIKeyboard);
    if (FAILED(result)) {
        OutputDebugStringA("Failed to set keyboard data format\n");
        return false;
    }

    result = _keyboard->SetCooperativeLevel(info.hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    if (FAILED(result)) {
        OutputDebugStringA("Failed to set keyboard cooperative level\n");
        return false;
    }

    result = _keyboard->Acquire();
    if (FAILED(result)) {
        OutputDebugStringA("Failed to acquire keyboard\n");
        return false;
    }

    // 마우스 장치 초기화
    result = _directInput->CreateDevice(GUID_SysMouse, &_mouse, nullptr);
    if (FAILED(result)) {
        OutputDebugStringA("Failed to create mouse device\n");
        return false;
    }

    result = _mouse->SetDataFormat(&c_dfDIMouse);
    if (FAILED(result)) {
        OutputDebugStringA("Failed to set mouse data format\n");
        return false;
    }

    result = _mouse->SetCooperativeLevel(info.hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    if (FAILED(result)) {
        OutputDebugStringA("Failed to set mouse cooperative level\n");
        return false;
    }

    result = _mouse->Acquire();
    if (FAILED(result)) {
        OutputDebugStringA("Failed to acquire mouse\n");
        return false;
    }

    // 마우스 시작 위치 설정
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(info.hwnd, &cursorPos);

    _mousePos = cursorPos;

    // 마우스 커서 로드
    _arrowCursor = LoadCursorFromFile(TEXT("..\\Resources\\MouseCursors\\Arrow.cur"));

    return true;
}

void Input::Shutdown() {
    if (_mouse) {
        _mouse->Unacquire();
        _mouse->Release();
        _mouse = nullptr;
    }

    if (_keyboard) {
        _keyboard->Unacquire();
        _keyboard->Release();
        _keyboard = nullptr;
    }

    if (_directInput) {
        _directInput->Release();
        _directInput = nullptr;
    }
}

bool Input::Update() {
    memcpy(_previousKeyboardState, _keyboardState, sizeof(_keyboardState));
    memcpy(&_previousMouseState, &_mouseState, sizeof(_mouseState));
    if (!ReadKeyboard() || !ReadMouse()) {
        return false;
    }
    SetCursor(_arrowCursor);

    ProcessInput();
    return true;
}

bool Input::ReadKeyboard() {
    HRESULT result = _keyboard->GetDeviceState(sizeof(_keyboardState), (LPVOID)&_keyboardState);
    if (FAILED(result)) {
        if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) {
            _keyboard->Acquire();
        }
        else {
            OutputDebugStringA("Failed to get keyboard state\n");
            return false;
        }
    }
    return true;
}

bool Input::ReadMouse() {
    HRESULT result = _mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&_mouseState);
    if (FAILED(result)) {
        if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) {
            _mouse->Acquire();
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

    _mousePos = cursorPos;

    // 마우스 휠 업데이트
    _mouseWheel += _mouseState.lZ;

    return true;
}

void Input::ProcessInput() {

    // 화면 경계 제한
    if (_mousePos.x < 0) { _mousePos.x = 0; }
    if (_mousePos.y < 0) { _mousePos.y = 0; }
    if (_mousePos.x > _screenWidth) { _mousePos.x = _screenWidth; }
    if (_mousePos.y > _screenHeight) { _mousePos.y = _screenHeight; }
}

bool Input::IsKeyPressed(unsigned char key) {
    return _keyboardState[key] & 0x80;
}

bool Input::IsKeyJustPressed(unsigned char key) {
    return (_keyboardState[key] & 0x80) && !(_previousKeyboardState[key] & 0x80);
}

bool Input::IsKeyJustReleased(unsigned char key) {
    return !(_keyboardState[key] & 0x80) && (_previousKeyboardState[key] & 0x80);
}

bool Input::IsMouseButtonPressed(int button) {
    return (_mouseState.rgbButtons[button] & 0x80) != 0;
}

bool Input::IsMouseButtonJustPressed(int button) {
    return (_mouseState.rgbButtons[button] & 0x80) && !(_previousMouseState.rgbButtons[button] & 0x80);
}

bool Input::IsMouseButtonJustReleased(int button) {
    return !(_mouseState.rgbButtons[button] & 0x80) && (_previousMouseState.rgbButtons[button] & 0x80);
}