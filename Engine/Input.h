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
    int GetMouseWheel() const { return m_mouseWheel; }
    POINT GetMousePos() const { return m_mousePos; }
private:
    bool ReadKeyboard();
    bool ReadMouse();
    void ProcessInput();

private:
    IDirectInput8* m_directInput = nullptr;
    IDirectInputDevice8* m_keyboard = nullptr;
    IDirectInputDevice8* m_mouse = nullptr;

    unsigned char m_keyboardState[256] = { 0 };
    unsigned char m_previousKeyboardState[256] = { 0 };  // 이전 프레임의 키보드 상태


    DIMOUSESTATE m_mouseState;
    DIMOUSESTATE m_previousMouseState;  // 이전 프레임의 마우스 상태

    int m_screenWidth = 0;
    int m_screenHeight = 0;

    POINT m_mousePos;

    int m_mouseWheel = 0;  // 마우스 휠 상태
};