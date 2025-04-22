#include "Level.h"
#include "math.h"
#include "render.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

Level::Level(const char* levelFile)
    : camera(nullptr)
    , isLoaded(false)
{
    camera = new Camera();
    
    // 加载关卡
    if (!load_tilemap(levelFile, &tilemap)) {
        std::cout << "Failed to load tilemap: " << levelFile << std::endl;
        return;
    }
    
    // 加载平台
    loadPlatforms();
    
    isLoaded = true;
}

Level::~Level()
{
    delete camera;
}

void Level::update(float dt)
{
    if (camera) {
        camera->update(dt);
    }
}

void Level::render()
{
    if (!isLoaded) return;

    // 应用相机变换
    if (camera) {
        camera->applyTransform();
    }

    // 渲染地形层
    for (int y = 0; y < tilemap.height; y++) {
        for (int x = 0; x < tilemap.width; x++) {
            if (y < tilemap.terrain.size() && x < tilemap.terrain[y].size() && tilemap.terrain[y][x] != 0) {
                draw_tile(&tilemap, tilemap.terrain[y][x], x * tilemap.tileSize, y * tilemap.tileSize);
            }
        }
    }

    // 渲染装饰层
    for (int y = 0; y < tilemap.height; y++) {
        for (int x = 0; x < tilemap.width; x++) {
            if (y < tilemap.decorations.size() && x < tilemap.decorations[y].size() && tilemap.decorations[y][x] != 0) {
                draw_tile(&tilemap, tilemap.decorations[y][x], x * tilemap.tileSize, y * tilemap.tileSize);
            }
        }
    }

    // 重置相机变换
    if (camera) {
        camera->resetTransform();
    }
}

bool Level::checkWallCollision(const Vec2& position, const Vec2& size, float& depth, bool horizontal)
{
    // 检查与平台的碰撞
    for (const auto& platform : platforms) {
        if (horizontal) {
            // 水平碰撞检测
            if (position.y + size.y > platform.position.y && 
                position.y < platform.position.y + platform.size.y) {
                if (position.x + size.x > platform.position.x && 
                    position.x < platform.position.x + platform.size.x) {
                    // 计算碰撞深度
                    float leftDepth = platform.position.x + platform.size.x - position.x;
                    float rightDepth = position.x + size.x - platform.position.x;
                    depth = (leftDepth < rightDepth) ? -leftDepth : rightDepth;
                    return true;
                }
            }
        } else {
            // 垂直碰撞检测
            if (position.x + size.x > platform.position.x && 
                position.x < platform.position.x + platform.size.x) {
                if (position.y + size.y > platform.position.y && 
                    position.y < platform.position.y + platform.size.y) {
                    // 计算碰撞深度
                    float topDepth = platform.position.y + platform.size.y - position.y;
                    float bottomDepth = position.y + size.y - platform.position.y;
                    depth = (topDepth < bottomDepth) ? -topDepth : bottomDepth;
                    return true;
                }
            }
        }
    }
    
    return false;
}

void Level::loadPlatforms()
{
    // 清除现有平台
    platforms.clear();

    // 从tilemap的地形层创建平台
    for (int y = 0; y < tilemap.height; y++) {
        for (int x = 0; x < tilemap.width; x++) {
            if (y < tilemap.terrain.size() && x < tilemap.terrain[y].size() && tilemap.terrain[y][x] != 0) {
                // 检查这个位置是否已经被处理过
                bool alreadyProcessed = false;
                for (const auto& platform : platforms) {
                    if (x * tilemap.tileSize >= platform.position.x && 
                        x * tilemap.tileSize < platform.position.x + platform.size.x &&
                        y * tilemap.tileSize >= platform.position.y && 
                        y * tilemap.tileSize < platform.position.y + platform.size.y) {
                        alreadyProcessed = true;
                        break;
                    }
                }

                if (!alreadyProcessed) {
                    // 找到水平方向上连续的相同地形块
                    int width = 1;
                    while (x + width < tilemap.width && 
                           y < tilemap.terrain.size() && 
                           x + width < tilemap.terrain[y].size() && 
                           tilemap.terrain[y][x + width] != 0) {
                        width++;
                    }

                    // 找到垂直方向上连续的相同地形块
                    int height = 1;
                    bool canExtendDown = true;
                    while (y + height < tilemap.height && canExtendDown) {
                        for (int i = 0; i < width; i++) {
                            if (y + height >= tilemap.terrain.size() || 
                                x + i >= tilemap.terrain[y + height].size() || 
                                tilemap.terrain[y + height][x + i] == 0) {
                                canExtendDown = false;
                                break;
                            }
                        }
                        if (canExtendDown) {
                            height++;
                        }
                    }

                    // 创建合并后的平台
                    Vec2 pos(x * tilemap.tileSize, y * tilemap.tileSize);
                    Vec2 size(width * tilemap.tileSize, height * tilemap.tileSize);
                    platforms.emplace_back(pos, size);
                }
            }
        }
    }
}

bool Level::checkCollision(const Vec2& pos, const Vec2& size) const {
    for (const auto& platform : platforms) {
        if (pos.x < platform.position.x + platform.size.x &&
            pos.x + size.x > platform.position.x &&
            pos.y < platform.position.y + platform.size.y &&
            pos.y + size.y > platform.position.y) {
            return true;
        }
    }
    return false;
}

float Level::getCollisionDepth(const Vec2& pos, const Vec2& size, const Platform& platform, bool isVertical) const {
    if (isVertical) {
        if (pos.x < platform.position.x + platform.size.x &&
            pos.x + size.x > platform.position.x) {
            if (pos.y + size.y > platform.position.y &&
                pos.y < platform.position.y) {
                return platform.position.y - (pos.y + size.y);
            }
            if (pos.y < platform.position.y + platform.size.y &&
                pos.y + size.y > platform.position.y + platform.size.y) {
                return (platform.position.y + platform.size.y) - pos.y;
            }
        }
    } else {
        if (pos.y < platform.position.y + platform.size.y &&
            pos.y + size.y > platform.position.y) {
            if (pos.x + size.x > platform.position.x &&
                pos.x < platform.position.x) {
                return platform.position.x - (pos.x + size.x);
            }
            if (pos.x < platform.position.x + platform.size.x &&
                pos.x + size.x > platform.position.x + platform.size.x) {
                return (platform.position.x + platform.size.x) - pos.x;
            }
        }
    }
    return 0.0f;
} 