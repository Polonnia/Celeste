#pragma once

#include "math.h"
#include <vector>
#include <string>
#include <map>

struct Tileset
{
    std::string name;
    std::string imagePath;
    int firstgid;  // 第一个图块的全局ID
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
    std::map<std::string, std::string> properties; // 存储地图属性
    std::vector<Tileset> tilesets;  // 图块集列表
};

// 加载地图数据
bool load_tilemap(const char* levelName, Tilemap* tilemap);

// 检查某个位置是否有地形
bool is_solid(const Tilemap* tilemap, float x, float y);

// 检查某个位置是否有尖刺
bool is_spike(const Tilemap* tilemap, float x, float y);

// 渲染地图
void render_tilemap(const Tilemap* tilemap); 