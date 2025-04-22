#include "Sprite.h"
#include <string>
#include <tchar.h>

Sprite::Sprite() : image(nullptr), width(0), height(0), 
                  frameWidth(0), frameHeight(0), frameCount(0),
                  currentFrame(0), frameTime(0.1f), elapsedTime(0),
                  spriteID(SPRITE_WHITE) {
}

Sprite::~Sprite() {
    if (image) {
        delete image;
        image = nullptr;
    }
}

bool Sprite::load(SpriteID id) {
    spriteID = id;
    SpriteData spriteData = get_sprite(id);
    
    // ʹ�� easyxpng.h �еĺ������� PNG ͼƬ
    image = new IMAGE;
    loadimage(image, _T("assets/textures/TEXTURE_ATLAS.png"), 0, 0, true);
    
    // ���ͼƬ�Ƿ�ɹ�����
    if (image->getwidth() == 0 || image->getheight() == 0) {
        delete image;
        image = nullptr;
        return false;
    }

    // ��ȡͼƬ�ߴ�
    width = image->getwidth();
    height = image->getheight();

    // ����֡�ߴ�
    frameWidth = spriteData.size.x;
    frameHeight = spriteData.size.y;
    frameCount = spriteData.frameCount;

    return true;
}

void Sprite::update(float deltaTime) {
    if (frameCount <= 1) return;

    elapsedTime += deltaTime;
    if (elapsedTime >= frameTime) {
        elapsedTime = 0;
        currentFrame = (currentFrame + 1) % frameCount;
    }
}

void Sprite::render(int x, int y, bool flipX) {
    if (!image) return;

    SpriteData spriteData = get_sprite(spriteID);
    int frameX = spriteData.atlasOffset.x + (currentFrame * frameWidth);
    int frameY = spriteData.atlasOffset.y;

    // ������ʱͼƬ���ڷ�ת
    IMAGE tempImage;
    tempImage.Resize(frameWidth, frameHeight);
    
    // ���Ƶ�ǰ֡����ʱͼƬ
    SetWorkingImage(&tempImage);
    putimage(0, 0, frameWidth, frameHeight, image, frameX, frameY);

    // �����Ҫ��ת
    if (flipX) {
        IMAGE flippedImage;
        flippedImage.Resize(frameWidth, frameHeight);
        SetWorkingImage(&flippedImage);
        for (int i = 0; i < frameWidth; i++) {
            for (int j = 0; j < frameHeight; j++) {
                SetWorkingImage(&tempImage);
                COLORREF color = getpixel(frameWidth - 1 - i, j);
                SetWorkingImage(&flippedImage);
                putpixel(i, j, color);
            }
        }
        SetWorkingImage(NULL);
        putimagePng(x, y, &flippedImage);
    } else {
        SetWorkingImage(NULL);
        putimagePng(x, y, &tempImage);
    }
}

void Sprite::setFrame(int frame) {
    if (frame >= 0 && frame < frameCount) {
        currentFrame = frame;
    }
} 