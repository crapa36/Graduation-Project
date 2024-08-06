#pragma once

enum class KEY_TYPE {
    UP = VK_UP,
    DOWN = VK_DOWN,
    LEFT = VK_LEFT,
    RIGHT = VK_RIGHT,
    SHIFT = VK_SHIFT,
    SPACE = VK_SPACE,
    CTRL = VK_CONTROL,
    ALT = VK_MENU,
    ESC = VK_ESCAPE,
    DEL = VK_DELETE,
    END = VK_END,
    PAGE_UP = VK_PRIOR,
    PAGE_DOWN = VK_NEXT,
    HOME = VK_HOME,
    INSERT = VK_INSERT,
    TAB = VK_TAB,
    BACKSPACE = VK_BACK,
    ENTER = VK_RETURN,
    LSHIFT = VK_LSHIFT,
    RSHIFT = VK_RSHIFT,
    LCTRL = VK_LCONTROL,
    RCTRL = VK_RCONTROL,
    LALT = VK_LMENU,
    RALT = VK_RMENU,
    CAPS = VK_CAPITAL,
    NUMLOCK = VK_NUMLOCK,
    SCROLL = VK_SCROLL,
    PRINT = VK_PRINT,
    PAUSE = VK_PAUSE,

    F1 = VK_F1,
    F2 = VK_F2,
    F3 = VK_F3,
    F4 = VK_F4,

    W = 'W',
    A = 'A',
    S = 'S',
    D = 'D',
    Q = 'Q',
    E = 'E',
    R = 'R',
    T = 'T',
    Y = 'Y',
    U = 'U',
    I = 'I',
    O = 'O',
    P = 'P',
    F = 'F',
    G = 'G',
    H = 'H',
    J = 'J',
    K = 'K',
    L = 'L',
    Z = 'Z',
    X = 'X',
    C = 'C',
    V = 'V',
    B = 'B',
    N = 'N',
    M = 'M',

    KEY_1 = '1',
    KEY_2 = '2',
    KEY_3 = '3',
    KEY_4 = '4',
    KEY_5 = '5',
    KEY_6 = '6',
    KEY_7 = '7',
    KEY_8 = '8',
    KEY_9 = '9',
    KEY_0 = '0',
    MIN = VK_OEM_MINUS,
    PLUS = VK_OEM_PLUS,

    LBUTTON = VK_LBUTTON,
    RBUTTON = VK_RBUTTON,
};

enum class KEY_STATE {
    NONE,
    PRESS,
    DOWN,
    UP,
    END
};

enum {
    KEY_TYPE_COUNT = static_cast<int32>(UINT8_MAX + 1),
    KEY_STATE_COUNT = static_cast<int32>(KEY_STATE::END),
};

class Input {
    DECLARE_SINGLETON(Input);
public:
    void Init(HWND hwnd);
    void Update();

    // 누르고 있을 때
    bool GetButton(KEY_TYPE key) { return GetState(key) == KEY_STATE::PRESS; }

    // 맨 처음 눌렀을 때
    bool GetButtonDown(KEY_TYPE key) { return GetState(key) == KEY_STATE::DOWN; }

    // 맨 처음 눌렀다 뗐을 때
    bool GetButtonUp(KEY_TYPE key) { return GetState(key) == KEY_STATE::UP; }
    const POINT& GetMousePos() { return _mousePos; }

private:
    inline KEY_STATE GetState(KEY_TYPE key) { return _states[static_cast<uint8>(key)]; }

private:
    HWND _hwnd;
    vector<KEY_STATE> _states;
    POINT _mousePos = {};
};
