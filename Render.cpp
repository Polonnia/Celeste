#include "render.h"
#include <graphics.h>
#include "easyxpng.h"
#include <map>
#include <tchar.h>
#include <iostream>  // ������ڵ������

// ȫ�ֱ������洢��ǰʹ�õ�����ͼ��
IMAGE* textureAtlas = nullptr;

// �洢�Ѽ��ص�ͼ�鼯ͼƬ
std::map<std::string, IMAGE*> loadedTilesets;

// ����任����
float cameraScale = 1.0f;
float cameraOffsetX = 0.0f;
float cameraOffsetY = 0.0f;

void draw_tile(const Tilemap* tilemap, int tileID, float x, float y)
{
    if (tileID == 0) return; // ��ͼ��

    std::cout << "Drawing tile ID: " << tileID << " at position (" << x << ", " << y << ")" << std::endl;

    // Ӧ������任
    x = (x - cameraOffsetX) * cameraScale;
    y = (y - cameraOffsetY) * cameraScale;

    // �ҵ���Ӧ��ͼ�鼯
    const Tileset* tileset = nullptr;
    for (const auto& ts : tilemap->tilesets)
    {
        std::cout << "Checking tileset with firstgid: " << ts.firstgid << ", tileCount: " << ts.tileCount << std::endl;
        if (tileID >= ts.firstgid && tileID < ts.firstgid + ts.tileCount)
        {
            tileset = &ts;
            std::cout << "Found matching tileset with image path: " << ts.imagePath << std::endl;
            break;
        }
    }

    if (!tileset) {
        std::cout << "No matching tileset found for tileID: " << tileID << std::endl;
        return;
    }

    // ȷ��ͼ�鼯ͼƬ�Ѽ���
    if (loadedTilesets.find(tileset->imagePath) == loadedTilesets.end())
    {
        IMAGE* image = new IMAGE;
        std::string fullPath = "assets/tiles/" + tileset->imagePath;
        std::cout << "Loading tileset image from: " << fullPath << std::endl;
        
        TCHAR imagePath[MAX_PATH];
#ifdef UNICODE
        MultiByteToWideChar(CP_UTF8, 0, fullPath.c_str(), -1, imagePath, MAX_PATH);
#else
        strcpy_s(imagePath, fullPath.c_str());
#endif
        loadimage(image, imagePath, 0, 0, true);
        
        // ���ͼƬ�Ƿ���سɹ�
        if (image->getwidth() > 0 && image->getheight() > 0)
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

    // ����ͼ����ͼ�鼯�е�λ��
    int localID = tileID - tileset->firstgid;
    int tileX = (localID % tileset->columns) * tileset->tileWidth;
    int tileY = (localID / tileset->columns) * tileset->tileHeight;

    std::cout << "Rendering tile from position (" << tileX << ", " << tileY << ") in tileset" << std::endl;

    // �������ź�ĳߴ�
    int scaledWidth = (int)(tileset->tileWidth * cameraScale);
    int scaledHeight = (int)(tileset->tileHeight * cameraScale);

    // ��Ⱦͼ��
    SetWorkingImage(NULL);
    putimagePng((int)x, (int)y, scaledWidth, scaledHeight, 
                loadedTilesets[tileset->imagePath], tileX, tileY);
}

void set_camera_transform(const Camera* camera)
{
    // ��ȡ����任����
    float transform[6];
    get_camera_transform(camera, transform);

    // ��������任
    cameraScale = transform[0];  // ����x��y��������ͬ
    cameraOffsetX = camera->position.x;
    cameraOffsetY = camera->position.y;
}

void reset_camera_transform()
{
    // ��������任
    cameraScale = 1.0f;
    cameraOffsetX = 0.0f;
    cameraOffsetY = 0.0f;
} 