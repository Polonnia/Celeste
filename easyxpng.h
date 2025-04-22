#pragma once

#include <graphics.h>

// 用于在EasyX中显示PNG格式图片的函数
inline void putimagePng(int x, int y, IMAGE* img) {
    DWORD* dst = GetImageBuffer();
    DWORD* src = GetImageBuffer(img);
    int src_width = img->getwidth();
    int src_height = img->getheight();
    int dst_width = getwidth();
    int dst_height = getheight();

    // 确保坐标在有效范围内
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x + src_width > dst_width) src_width = dst_width - x;
    if (y + src_height > dst_height) src_height = dst_height - y;

    // 复制图像数据
    for (int i = 0; i < src_height; i++) {
        for (int j = 0; j < src_width; j++) {
            dst[(y + i) * dst_width + (x + j)] = src[i * src_width + j];
        }
    }
}

// 带源矩形区域的PNG图片显示函数
inline void putimagePng(int x, int y, int width, int height, IMAGE* img, int srcX, int srcY) {
    DWORD* dst = GetImageBuffer();
    DWORD* src = GetImageBuffer(img);
    int src_width = img->getwidth();
    int dst_width = getwidth();
    int dst_height = getheight();

    // 确保坐标和尺寸在有效范围内
    if (x < 0) { width += x; srcX -= x; x = 0; }
    if (y < 0) { height += y; srcY -= y; y = 0; }
    if (x + width > dst_width) width = dst_width - x;
    if (y + height > dst_height) height = dst_height - y;
    if (srcX < 0) { width += srcX; x -= srcX; srcX = 0; }
    if (srcY < 0) { height += srcY; y -= srcY; srcY = 0; }
    if (width <= 0 || height <= 0) return;

    // 复制图像数据
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            dst[(y + i) * dst_width + (x + j)] = src[(srcY + i) * src_width + (srcX + j)];
        }
    }
} 