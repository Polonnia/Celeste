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

// �洢�Ѽ��صı���ͼƬ
std::map<std::string, IMAGE*> loadedBackgrounds;

// ����任����
float cameraScale = 1.0f;
float cameraOffsetX = 0.0f;
float cameraOffsetY = 0.0f;

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

void render_backgrounds(const Tilemap* tilemap)
{
    // ��Ⱦ�󾰲�
    for (const auto& bg : tilemap->backgrounds)
    {
        IMAGE bgImage;
        // 1. �� std::string ת��Ϊ TCHAR �ַ���
        TCHAR imagePath[MAX_PATH];
        size_t converted = 0;
        mbstowcs_s(&converted, imagePath, bg.imagePath.c_str(), _TRUNCATE);

        try {
            // 2. ����ͼƬ
            loadimage(&bgImage, imagePath);
            
            // ��ȡͼƬ�ߴ�
            int imageWidth = bgImage.getwidth();
            int imageHeight = bgImage.getheight();
            
            // ����ʵ����Ⱦλ�ã��������λ�ú��Ӳ
            float renderX = bg.offsetX - cameraOffsetX * bg.parallaxFactor;
            
            // ������Ҫ�ظ���Ⱦ�Ĵ���
            int screenWidth = 1280; // ������Ļ���Ϊ1280
            int repeatCount = (screenWidth / imageWidth) + 3; // ����ȾһЩ��ȷ������
            
            // ��Ⱦ����ͼƬ
            for (int i = -1; i < repeatCount; i++) {
                float currentX = renderX + i * imageWidth;
                
                // ֻ��Ⱦ����Ļ��Χ�ڵĲ���
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
    // ��Ⱦ������
    IMAGE bgImage;
    // 1. �� std::string ת�� TCHAR ���ַ�����
    TCHAR imagePath[MAX_PATH];
    size_t converted = 0;
    mbstowcs_s(&converted, imagePath, tilemap->backgroundImagePath.c_str(), _TRUNCATE);

    // 2. ���� loadimage
    loadimage(&bgImage, imagePath);
    putimagePng(0, 0, &bgImage);
}

void render_tilemap(const Tilemap* tilemap)
{
    // ��Ⱦ��
    render_backgrounds(tilemap);
    
    // ��Ⱦ������
    render_main_background(tilemap);
    
    // ��Ⱦ���β�
    // ... existing rendering code ...
} 