#include "renderer.h"
#include "render_interface.h"
#include "easyXPng.h"
#include <fstream>
#include <ctime>

// 全局渲染器上下文
EasyXContext easyxContext;

// 初始化EasyX渲染器
bool easyx_init() {
    // 初始化图形窗口
    initgraph(input->screenSize.x, input->screenSize.y);
    setbkcolor(RGB(119, 33, 111));
    cleardevice();
    
    // 加载纹理图集
    if (!load_texture("assets/textures/TEXTURE_ATLAS.png")) {
        return false;
    }
    
    // 加载字体
    if (!load_font("assets/fonts/AtariClassic-gry3.ttf", 8)) {
        return false;
    }
    
    return true;
}

// 加载纹理
bool load_texture(const char* path) {
    // 检查文件是否存在
    std::ifstream file(path);
    if (!file.good()) {
        SM_ASSERT(false, "Failed to load texture: %s", path);
        return false;
    }
    
    // 加载纹理
    easyxContext.textureAtlas = new IMAGE;
    loadimage(easyxContext.textureAtlas, path);
    
    // 更新时间戳
    easyxContext.textureTimestamp = std::time(nullptr);
    
    return true;
}

// 加载字体
bool load_font(const char* path, int fontSize) {
    // EasyX使用系统字体，这里我们使用默认字体
    // 设置字体大小
    LOGFONT font;
    gettextstyle(&font);
    font.lfHeight = fontSize;
    font.lfWidth = 0;
    font.lfWeight = FW_NORMAL;
    _tcscpy_s(font.lfFaceName, _T("Consolas"));
    settextstyle(&font);
    
    return true;
}

// 绘制精灵
void draw_sprite(const Vec2& position, const Vec2& size, const Vec4& color, 
                const Vec2& texCoord, const Vec2& texSize) {
    // 设置颜色
    setfillcolor(RGB(color.x * 255, color.y * 255, color.z * 255));
    
    // 创建临时图像用于存储纹理区域
    IMAGE tempImage;
    tempImage.Resize(size.x, size.y);
    
    // 从纹理图集中复制区域
    DWORD* dst = GetImageBuffer(&tempImage);
    DWORD* src = GetImageBuffer(easyxContext.textureAtlas);
    int srcWidth = easyxContext.textureAtlas->getwidth();
    
    for (int y = 0; y < size.y; y++) {
        for (int x = 0; x < size.x; x++) {
            int srcX = texCoord.x + x;
            int srcY = texCoord.y + y;
            dst[y * size.x + x] = src[srcY * srcWidth + srcX];
        }
    }
    
    // 使用putimagepng绘制图像，支持alpha通道
    putimagepng(position.x, position.y, &tempImage);
}

// 绘制文本
void draw_text(const char* text, const Vec2& position, const Vec4& color) {
    // 设置文本颜色
    settextcolor(RGB(color.x * 255, color.y * 255, color.z * 255));
    
    // 绘制文本
    outtextxy(position.x, position.y, text);
}

// 渲染函数
void easyx_render() {
    // 清空屏幕
    cleardevice();
    
    // 渲染所有材质
    for (int i = 0; i < renderData->materials.count; i++) {
        Material& material = renderData->materials.elements[i];
        Transform& transform = renderData->transforms.elements[i];
        
        draw_sprite(
            transform.pos,
            transform.size,
            material.color,
            transform.atlasOffset,
            transform.spriteSize
        );
    }
    
    // 渲染所有文本
    for (int i = 0; i < renderData->texts.count; i++) {
        Text& text = renderData->texts.elements[i];
        draw_text(text.text, text.position, text.color);
    }
    
    // 刷新屏幕
    FlushBatchDraw();
}

// 清理资源
void easyx_cleanup() {
    // 释放纹理资源
    if (easyxContext.textureAtlas) {
        delete easyxContext.textureAtlas;
        easyxContext.textureAtlas = nullptr;
    }
    
    if (easyxContext.fontAtlas) {
        delete easyxContext.fontAtlas;
        easyxContext.fontAtlas = nullptr;
    }
    
    // 关闭图形窗口
    closegraph();
} 