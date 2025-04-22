#pragma once
#include <graphics.h>
#include "easyxpng.h"
#include <string>
#include "assets.h"

class Sprite {
private:
    IMAGE* image;
    int width;
    int height;
    int frameWidth;
    int frameHeight;
    int frameCount;
    int currentFrame;
    float frameTime;
    float elapsedTime;
    SpriteID spriteID;

public:
    Sprite();
    ~Sprite();

    bool load(SpriteID id);
    void update(float deltaTime);
    void render(int x, int y, bool flipX = false);
    void setFrame(int frame);
    int getWidth() const { return frameWidth > 0 ? frameWidth : width; }
    int getHeight() const { return frameHeight > 0 ? frameHeight : height; }
}; 