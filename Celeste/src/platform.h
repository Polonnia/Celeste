#pragma once

// 游戏输入类型
enum GameInputType {
    GAME_INPUT_LEFT,
    GAME_INPUT_RIGHT,
    GAME_INPUT_JUMP,
    GAME_INPUT_DASH,
    GAME_INPUT_MENU,
    GAME_INPUT_COUNT
};

// 游戏输入状态
struct GameInput {
    struct {
        bool is_down;
        bool just_pressed;
    } buttons[GAME_INPUT_COUNT];
};

// 平台相关函数声明
void platform_create_window(const char* title);
void platform_update_window();
GameInput* platform_get_input();
void platform_get_window_size(int* width, int* height);
bool platform_is_running();
void platform_close_window();
double platform_get_time();
char* platform_load_file(const char* path, int* size);
void platform_free_file(char* file); 