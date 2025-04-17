#include <windows.h>
#include <easyx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "platform.h"
#include "game.h"

// 全局变量
static HWND window;
static HDC hdc;
static bool running = true;
static GameInput game_input = {};
static int window_width = 1280;
static int window_height = 720;

// 键码映射表
static int keycode_lookup_table[256] = {};

// 初始化键码映射表
static void init_keycode_lookup_table() {
    keycode_lookup_table[VK_LEFT] = GAME_INPUT_LEFT;
    keycode_lookup_table[VK_RIGHT] = GAME_INPUT_RIGHT;
    keycode_lookup_table[VK_UP] = GAME_INPUT_JUMP;
    keycode_lookup_table[VK_SPACE] = GAME_INPUT_JUMP;
    keycode_lookup_table['Z'] = GAME_INPUT_JUMP;
    keycode_lookup_table['X'] = GAME_INPUT_DASH;
    keycode_lookup_table['C'] = GAME_INPUT_DASH;
    keycode_lookup_table[VK_ESCAPE] = GAME_INPUT_MENU;
}

// 窗口消息处理函数
LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_CLOSE:
            running = false;
            break;
        case WM_KEYDOWN:
            if (wparam < 256) {
                int game_input_type = keycode_lookup_table[wparam];
                if (game_input_type) {
                    game_input.buttons[game_input_type].just_pressed = true;
                    game_input.buttons[game_input_type].is_down = true;
                }
            }
            break;
        case WM_KEYUP:
            if (wparam < 256) {
                int game_input_type = keycode_lookup_table[wparam];
                if (game_input_type) {
                    game_input.buttons[game_input_type].is_down = false;
                }
            }
            break;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

// 创建窗口
void platform_create_window(const char* title) {
    // 注册窗口类
    WNDCLASS wc = {};
    wc.lpfnWndProc = window_proc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "CelesteClone";
    RegisterClass(&wc);

    // 创建窗口
    window = CreateWindowEx(
        0,
        "CelesteClone",
        title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        window_width, window_height,
        NULL, NULL,
        GetModuleHandle(NULL),
        NULL
    );

    // 初始化EasyX
    initgraph(window_width, window_height);
    hdc = GetDC(window);

    // 显示窗口
    ShowWindow(window, SW_SHOW);
    UpdateWindow(window);

    // 初始化键码映射表
    init_keycode_lookup_table();
}

// 更新窗口
void platform_update_window() {
    // 处理消息
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 更新游戏输入状态
    for (int i = 0; i < GAME_INPUT_COUNT; i++) {
        game_input.buttons[i].just_pressed = false;
    }

    // 清屏
    cleardevice();

    // 更新游戏
    simulate();

    // 刷新屏幕
    FlushBatchDraw();
}

// 获取游戏输入
GameInput* platform_get_input() {
    return &game_input;
}

// 获取窗口尺寸
void platform_get_window_size(int* width, int* height) {
    *width = window_width;
    *height = window_height;
}

// 检查是否正在运行
bool platform_is_running() {
    return running;
}

// 关闭窗口
void platform_close_window() {
    closegraph();
    DestroyWindow(window);
}

// 获取时间
double platform_get_time() {
    static LARGE_INTEGER frequency;
    static LARGE_INTEGER start_time;
    static bool initialized = false;

    if (!initialized) {
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&start_time);
        initialized = true;
    }

    LARGE_INTEGER current_time;
    QueryPerformanceCounter(&current_time);
    return (double)(current_time.QuadPart - start_time.QuadPart) / frequency.QuadPart;
}

// 加载文件
char* platform_load_file(const char* path, int* size) {
    FILE* file = fopen(path, "rb");
    if (!file) return NULL;

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(*size + 1);
    fread(buffer, 1, *size, file);
    buffer[*size] = 0;

    fclose(file);
    return buffer;
}

// 释放文件内存
void platform_free_file(char* file) {
    free(file);
}