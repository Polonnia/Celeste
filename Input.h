#pragma once
#include <graphics.h>
#include <windows.h>

enum class KeyCodeID {
    KEY_A = 'A',
    KEY_B = 'B',
    KEY_C = 'C',
    KEY_D = 'D',
    KEY_E = 'E',
    KEY_F = 'F',
    KEY_G = 'G',
    KEY_H = 'H',
    KEY_I = 'I',
    KEY_J = 'J',
    KEY_K = 'K',
    KEY_L = 'L',
    KEY_M = 'M',
    KEY_N = 'N',
    KEY_O = 'O',
    KEY_P = 'P',
    KEY_Q = 'Q',
    KEY_R = 'R',
    KEY_S = 'S',
    KEY_T = 'T',
    KEY_U = 'U',
    KEY_V = 'V',
    KEY_W = 'W',
    KEY_X = 'X',
    KEY_Y = 'Y',
    KEY_Z = 'Z',
    KEY_SPACE = VK_SPACE,
    KEY_ESCAPE = VK_ESCAPE,
    KEY_UP = VK_UP,
    KEY_DOWN = VK_DOWN,
    KEY_LEFT = VK_LEFT,
    KEY_RIGHT = VK_RIGHT,
    KEY_SHIFT = VK_SHIFT,
    KEY_COUNT = 256  // 使用一个足够大的值来包含所有可能的键
};

struct Key {
    bool isDown;
    bool justPressed;
    bool justReleased;
    unsigned char halfTransitionCount;
};

struct Input {
    int screenWidth;
    int screenHeight;
    Key keys[static_cast<int>(KeyCodeID::KEY_COUNT)];
};

extern Input* input;

// Input functions
bool key_pressed_this_frame(KeyCodeID key);
bool key_released_this_frame(KeyCodeID key);
bool key_is_down(KeyCodeID key);
void update_input(); 