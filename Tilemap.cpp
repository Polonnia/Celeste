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

        // ���ñ���ͼƬ·��
        tilemap->backgroundImagePath = "assets/levels/" + std::string(levelName) + "/terrain.png";
        std::cout << "Background image path: " << tilemap->backgroundImagePath << std::endl;

        // ���غ�ͼƬ
        tilemap->backgrounds.clear();
        
        // ��������󾰲㣬�Ӳ�ϵ���ֱ�Ϊ0.3��0.5
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
    // ����ÿ���󾰲��ƫ����
    for (auto& bg : tilemap->backgrounds)
    {
        // �����Ӳ�ϵ��������ƶ��������ƫ����
        bg.offsetX -= playerDeltaX * bg.parallaxFactor;
        
        // ȷ��ƫ�����ں���Χ��
        if (bg.offsetX < -1280) bg.offsetX += 1280;
        if (bg.offsetX > 1280) bg.offsetX -= 1280;
    }
} 