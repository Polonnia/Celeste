#pragma once

#include "math.h"
#include <vector>
#include <string>
#include <map>

struct ParallaxBackground {
    std::string imagePath;
    float parallaxFactor; // �Ӳ�ϵ����ֵԽС�ƶ�Խ��
    float offsetX;        // ��ǰƫ����
};

struct Tilemap
{
    std::string backgroundImagePath; // ����ͼƬ·��
    
    // ��ͼƬ
    std::vector<ParallaxBackground> backgrounds;
};

// ���ص�ͼ����
bool load_tilemap(const char* levelName, Tilemap* tilemap);

// ���ĳ��λ���Ƿ��е���
//bool is_solid(const Tilemap* tilemap, float x, float y);

// ���ĳ��λ���Ƿ��м��
//bool is_spike(const Tilemap* tilemap, float x, float y);

// ��Ⱦ��ͼ
void render_tilemap(const Tilemap* tilemap);

// ���º�ͼƬλ��
void update_backgrounds(Tilemap* tilemap, float playerX, float playerDeltaX); 