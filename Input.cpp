#include "Input.h"
#include <graphics.h>
#include <windows.h>

Input* input = new Input();

// Current frame input state
static bool currentKeys[static_cast<int>(KeyCodeID::KEY_COUNT)] = { false };
// Previous frame input state
static bool previousKeys[static_cast<int>(KeyCodeID::KEY_COUNT)] = { false };

void update_input() {
    // Copy current keys to previous keys
    for (int i = 0; i < static_cast<int>(KeyCodeID::KEY_COUNT); i++) {
        previousKeys[i] = currentKeys[i];
    }

    // Update current keys
    for (int i = 0; i < static_cast<int>(KeyCodeID::KEY_COUNT); i++) {
        currentKeys[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
    }
}

bool key_pressed_this_frame(KeyCodeID key) {
    int keyIndex = static_cast<int>(key);
    return currentKeys[keyIndex] && !previousKeys[keyIndex];
}

bool key_released_this_frame(KeyCodeID key) {
    int keyIndex = static_cast<int>(key);
    return !currentKeys[keyIndex] && previousKeys[keyIndex];
}

bool key_is_down(KeyCodeID key) {
    int keyIndex = static_cast<int>(key);
    return currentKeys[keyIndex];
} 