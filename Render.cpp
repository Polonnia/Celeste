#include "render.h"
#include <graphics.h>
#include "easyxpng.h"
#include <map>
#include <tchar.h>
#include <iostream>  // 添加用于调试输出

// 全局变量，存储当前使用的纹理图集
IMAGE* textureAtlas = nullptr;

// 存储已加载的图块集图片
std::map<std::string, IMAGE*> loadedTilesets;

// 存储已加载的背景图片
std::map<std::string, IMAGE*> loadedBackgrounds;

// 相机变换矩阵
float cameraScale = 1.0f;
float cameraOffsetX = 0.0f;
float cameraOffsetY = 0.0f;

void set_camera_transform(const Camera* camera)
{
    // 获取相机变换矩阵
    float transform[6];
    get_camera_transform(camera, transform);

    // 设置相机变换
    cameraScale = transform[0];  // 假设x和y的缩放相同
    cameraOffsetX = camera->position.x;
    cameraOffsetY = camera->position.y;
}

void reset_camera_transform()
{
    // 重置相机变换
    cameraScale = 1.0f;
    cameraOffsetX = 0.0f;
    cameraOffsetY = 0.0f;
}

void render_backgrounds(const Tilemap* tilemap)
{
    // 渲染后景层
    for (const auto& bg : tilemap->backgrounds)
    {
        IMAGE bgImage;
        // 1. 将 std::string 转换为 TCHAR 字符串
        TCHAR imagePath[MAX_PATH];
        size_t converted = 0;
        mbstowcs_s(&converted, imagePath, bg.imagePath.c_str(), _TRUNCATE);

        try {
            // 2. 加载图片
            loadimage(&bgImage, imagePath);
            
            // 获取图片尺寸
            int imageWidth = bgImage.getwidth();
            int imageHeight = bgImage.getheight();
            
            // 计算实际渲染位置（考虑相机位置和视差）
            float renderX = bg.offsetX - cameraOffsetX * bg.parallaxFactor;
            
            // 计算需要重复渲染的次数
            int screenWidth = 1280; // 假设屏幕宽度为1280
            int repeatCount = (screenWidth / imageWidth) + 3; // 多渲染一些以确保覆盖
            
            // 渲染背景图片
            for (int i = -1; i < repeatCount; i++) {
                float currentX = renderX + i * imageWidth;
                
                // 只渲染在屏幕范围内的部分
                if (currentX + imageWidth > 0 && currentX < screenWidth) {
                    putimagePng((int)currentX, 0, &bgImage);
                }
            }
        }
        catch (...) {
            std::cout << "Failed to load background image: " << bg.imagePath << std::endl;
        }
    }
}

void render_main_background(const Tilemap* tilemap)
{
    // 渲染主背景
    IMAGE bgImage;
    // 1. 把 std::string 转成 TCHAR 宽字符数组
    TCHAR imagePath[MAX_PATH];
    size_t converted = 0;
    mbstowcs_s(&converted, imagePath, tilemap->backgroundImagePath.c_str(), _TRUNCATE);

    // 2. 调用 loadimage
    loadimage(&bgImage, imagePath);
    putimagePng(0, 0, &bgImage);
}

void render_tilemap(const Tilemap* tilemap)
{
    // 渲染后景
    render_backgrounds(tilemap);
    
    // 渲染主背景
    render_main_background(tilemap);
    
    // 渲染地形层
    // ... existing rendering code ...
} 