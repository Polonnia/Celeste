#pragma once

#include "math.h"
#include <vector>
#include <string>
#include <map>

struct ParallaxBackground {
    std::string imagePath;
    float parallaxFactor; // 视差系数，值越小移动越慢
    float offsetX;        // 当前偏移量
};

struct Tilemap
{
    std::string backgroundImagePath; // 背景图片路径
    
    // 后景图片
    std::vector<ParallaxBackground> backgrounds;
};

// 加载地图数据
bool load_tilemap(const char* levelName, Tilemap* tilemap);

// 检查某个位置是否有地形
//bool is_solid(const Tilemap* tilemap, float x, float y);

// 检查某个位置是否有尖刺
//bool is_spike(const Tilemap* tilemap, float x, float y);

// 渲染地图
void render_tilemap(const Tilemap* tilemap);

// 更新后景图片位置
void update_backgrounds(Tilemap* tilemap, float playerX, float playerDeltaX); 