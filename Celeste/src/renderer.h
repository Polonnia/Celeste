#pragma once
#include <graphics.h>
#include "render_interface.h"

// 渲染器上下文
struct EasyXContext {
    IMAGE* textureAtlas;        // 纹理图集
    IMAGE* fontAtlas;           // 字体图集
    long long textureTimestamp; // 纹理时间戳
};

// 全局渲染器上下文
extern EasyXContext easyxContext;

// 初始化EasyX渲染器
bool easyx_init();

// 渲染函数
void easyx_render();

// 加载纹理
bool load_texture(const char* path);

// 加载字体
bool load_font(const char* path, int fontSize);

// 绘制精灵
void draw_sprite(const Vec2& position, const Vec2& size, const Vec4& color, 
                const Vec2& texCoord, const Vec2& texSize);

// 绘制文本
void draw_text(const char* text, const Vec2& position, const Vec4& color);

// 清理资源
void easyx_cleanup(); 