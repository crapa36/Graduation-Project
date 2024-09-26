#pragma once
#include <dinput.h>

class Input {
public:
    DECLARE_SINGLETON(Input);

    bool Init(WindowInfo info);
    void Shutdown();
    bool Update();

    bool IsKeyPressed(unsigned char key);
    bool IsKeyJustPressed(unsigned char key);
    bool IsKeyJustReleased(unsigned char key);
    bool IsMouseButtonPressed(int button);
    bool IsMouseButtonJustPressed(int button);
    bool IsMouseButtonJustReleased(int button);

    // 마우스 휠 값 가져오기
    int GetMouseWheel() const { return _mouseWheel; }
    POINT GetMousePos() const { return _mousePos; }
private:
    bool ReadKeyboard();
    bool ReadMouse();
    void ProcessInput();

private:
    IDirectInput8* _directInput = nullptr;
    IDirectInputDevice8* _keyboard = nullptr;
    IDirectInputDevice8* _mouse = nullptr;

    unsigned char _keyboardState[256] = { 0 };
    unsigned char _previousKeyboardState[256] = { 0 };  // 이전 프레임의 키보드 상태

    DIMOUSESTATE _mouseState;
    DIMOUSESTATE _previousMouseState;  // 이전 프레임의 마우스 상태

    int _screenWidth = 0;
    int _screenHeight = 0;

    POINT _mousePos;

    int _mouseWheel = 0;  // 마우스 휠 상태

    HCURSOR _arrowCursor = nullptr;
};