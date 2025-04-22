#pragma once

#include "math.h"
#include "Player.h"
#include "Camera.h"
#include "Platform.h"
#include "Tilemap.h"
#include <vector>

class Level {
public:
    Level(const char* levelFile);
    ~Level();

    void update(float dt);
    void render();

    // Collision detection
    bool checkWallCollision(const Vec2& position, const Vec2& size, float& depth, bool horizontal);
    bool checkCollision(const Vec2& pos, const Vec2& size) const;
    float getCollisionDepth(const Vec2& pos, const Vec2& size, const Platform& platform, bool isVertical) const;
    
    // Getters
    const std::vector<Platform>& getPlatforms() const { return platforms; }
    const Camera* getCamera() const { return camera; }
    Camera* getCamera() { return camera; }

private:
    Tilemap tilemap;
    Camera* camera;
    std::vector<Platform> platforms;
    bool isLoaded;

    void loadPlatforms();  // 从tilemap加载平台
}; 