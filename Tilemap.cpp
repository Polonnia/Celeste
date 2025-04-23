#include "Tilemap.h"
#include "assets.h"
#include "render.h"
#include "easyxpng.h"
#include <tchar.h>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <iostream>
#include "../single_include/nlohmann/json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

bool load_tilemap(const char* levelName, Tilemap* tilemap)
{
    try {

        // 设置背景图片路径
        tilemap->backgroundImagePath = "assets/levels/" + std::string(levelName) + "/terrain.png";
        std::cout << "Background image path: " << tilemap->backgroundImagePath << std::endl;

        // 加载后景图片
        tilemap->backgrounds.clear();
        
        // 添加两个后景层，视差系数分别为0.3和0.5
        ParallaxBackground bg1;
        bg1.imagePath = "assets/levels/" + std::string(levelName) + "/bg1.png";
        bg1.parallaxFactor = 0.3f;
        bg1.offsetX = 0.0f;
        tilemap->backgrounds.push_back(bg1);

        ParallaxBackground bg2;
        bg2.imagePath = "assets/levels/" + std::string(levelName) + "/bg2.png";
        bg2.parallaxFactor = 0.5f;
        bg2.offsetX = 0.0f;
        tilemap->backgrounds.push_back(bg2);

        std::cout << "Successfully loaded tilemap" << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cout << "Error loading tilemap: " << e.what() << std::endl;
        return false;
    }
}


void update_backgrounds(Tilemap* tilemap, float playerX, float playerDeltaX)
{
    // 更新每个后景层的偏移量
    for (auto& bg : tilemap->backgrounds)
    {
        // 根据视差系数和玩家移动距离更新偏移量
        bg.offsetX -= playerDeltaX * bg.parallaxFactor;
        
        // 确保偏移量在合理范围内
        if (bg.offsetX < -1280) bg.offsetX += 1280;
        if (bg.offsetX > 1280) bg.offsetX -= 1280;
    }
} 