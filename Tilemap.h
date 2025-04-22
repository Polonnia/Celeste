#pragma once

#include "math.h"
#include <vector>
#include <string>
#include <map>

struct Tileset
{
    std::string name;
    std::string imagePath;
    int firstgid;  // ��һ��ͼ���ȫ��ID
    int tileWidth;
    int tileHeight;
    int imageWidth;
    int imageHeight;
    int columns;
    int tileCount;
};

struct Tilemap
{
    std::vector<std::vector<int>> terrain;
    std::vector<std::vector<int>> decorations;
    std::vector<std::vector<int>> spikes;
    int width = 0;
    int height = 0;
    float tileSize = 16.0f;
    std::map<std::string, std::string> properties; // �洢��ͼ����
    std::vector<Tileset> tilesets;  // ͼ�鼯�б�
};

// ���ص�ͼ����
bool load_tilemap(const char* levelName, Tilemap* tilemap);

// ���ĳ��λ���Ƿ��е���
bool is_solid(const Tilemap* tilemap, float x, float y);

// ���ĳ��λ���Ƿ��м��
bool is_spike(const Tilemap* tilemap, float x, float y);

// ��Ⱦ��ͼ
void render_tilemap(const Tilemap* tilemap); 