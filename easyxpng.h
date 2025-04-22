#pragma once

#include <graphics.h>

// ������EasyX����ʾPNG��ʽͼƬ�ĺ���
inline void putimagePng(int x, int y, IMAGE* img) {
    DWORD* dst = GetImageBuffer();
    DWORD* src = GetImageBuffer(img);
    int src_width = img->getwidth();
    int src_height = img->getheight();
    int dst_width = getwidth();
    int dst_height = getheight();

    // ȷ����������Ч��Χ��
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x + src_width > dst_width) src_width = dst_width - x;
    if (y + src_height > dst_height) src_height = dst_height - y;

    // ����ͼ������
    for (int i = 0; i < src_height; i++) {
        for (int j = 0; j < src_width; j++) {
            dst[(y + i) * dst_width + (x + j)] = src[i * src_width + j];
        }
    }
}

// ��Դ���������PNGͼƬ��ʾ����
inline void putimagePng(int x, int y, int width, int height, IMAGE* img, int srcX, int srcY) {
    DWORD* dst = GetImageBuffer();
    DWORD* src = GetImageBuffer(img);
    int src_width = img->getwidth();
    int dst_width = getwidth();
    int dst_height = getheight();

    // ȷ������ͳߴ�����Ч��Χ��
    if (x < 0) { width += x; srcX -= x; x = 0; }
    if (y < 0) { height += y; srcY -= y; y = 0; }
    if (x + width > dst_width) width = dst_width - x;
    if (y + height > dst_height) height = dst_height - y;
    if (srcX < 0) { width += srcX; x -= srcX; srcX = 0; }
    if (srcY < 0) { height += srcY; y -= srcY; srcY = 0; }
    if (width <= 0 || height <= 0) return;

    // ����ͼ������
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            dst[(y + i) * dst_width + (x + j)] = src[(srcY + i) * src_width + (srcX + j)];
        }
    }
} 