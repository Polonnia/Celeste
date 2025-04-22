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

// 相机变换矩阵
float cameraScale = 1.0f;
float cameraOffsetX = 0.0f;
float cameraOffsetY = 0.0f;

void draw_tile(const Tilemap* tilemap, int tileID, float x, float y)
{
    if (tileID == 0) return; // 空图块

    std::cout << "Drawing tile ID: " << tileID << " at position (" << x << ", " << y << ")" << std::endl;

    // 应用相机变换
    x = (x - cameraOffsetX) * cameraScale;
    y = (y - cameraOffsetY) * cameraScale;

    // 找到对应的图块集
    const Tileset* tileset = nullptr;
    for (const auto& ts : tilemap->tilesets)
    {
        std::cout << "Checking tileset with firstgid: " << ts.firstgid << std::endl;
        if (tileID >= ts.firstgid)
        {
            // 找到最后一个匹配的tileset
            tileset = &ts;
        }
    }

    if (!tileset) {
        std::cout << "No matching tileset found for tileID: " << tileID << std::endl;
        return;
    }

    // 确保图块集图片已加载
    if (loadedTilesets.find(tileset->imagePath) == loadedTilesets.end())
    {
        IMAGE* image = new IMAGE;
        std::string fullPath = tileset->imagePath; // 直接使用完整路径
        std::cout << "Loading tileset image from: " << fullPath << std::endl;
        
        TCHAR imagePath[MAX_PATH];
#ifdef UNICODE
        MultiByteToWideChar(CP_UTF8, 0, fullPath.c_str(), -1, imagePath, MAX_PATH);
#else
        strcpy_s(imagePath, fullPath.c_str());
#endif
        
        // 尝试加载图片
        bool loadSuccess = false;
        try {
            loadimage(image, imagePath, 0, 0, true);
            if (image->getwidth() > 0 && image->getheight() > 0) {
                loadSuccess = true;
            }
        } catch (...) {
            std::cout << "Exception while loading image: " << fullPath << std::endl;
        }
        
        // 检查图片是否加载成功
        if (loadSuccess)
        {
            std::cout << "Successfully loaded image with dimensions: " << image->getwidth() << "x" << image->getheight() << std::endl;
            loadedTilesets[tileset->imagePath] = image;
        }
        else
        {
            std::cout << "Failed to load tileset image: " << fullPath << std::endl;
            delete image;
            return;
        }
    }

    // 计算图块在图块集中的位置
    int localID = tileID - tileset->firstgid;
    
    // 使用默认的tileWidth和tileHeight
    int tileWidth = tileset->tileWidth;
    int tileHeight = tileset->tileHeight;
    
    // 如果tileWidth或tileHeight为0，使用默认值
    if (tileWidth <= 0) tileWidth = 8; // 使用地图的默认tilewidth
    if (tileHeight <= 0) tileHeight = 8; // 使用地图的默认tileheight
    
    // 假设每个tileset有10列
    int columns = 10;
    if (tileset->columns > 0) columns = tileset->columns;
    
    int tileX = (localID % columns) * tileWidth;
    int tileY = (localID / columns) * tileHeight;

    std::cout << "Rendering tile from position (" << tileX << ", " << tileY << ") in tileset" << std::endl;

    // 计算缩放后的尺寸
    int scaledWidth = (int)(tileWidth * cameraScale);
    int scaledHeight = (int)(tileHeight * cameraScale);

    // 渲染图块
    SetWorkingImage(NULL);
    putimagePng((int)x, (int)y, scaledWidth, scaledHeight, 
                loadedTilesets[tileset->imagePath], tileX, tileY);
}

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