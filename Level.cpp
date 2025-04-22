#include "Level.h"
#include "math.h"
#include "render.h"
#include <string>
#include <fstream>
#include <sstream>

Level::Level(const char* levelFile)
    : camera(nullptr)
    , isLoaded(false)
{
    camera = new Camera();
    
    // ���عؿ�
    if (!load_tilemap(levelFile, &tilemap)) {
        return;
    }
    
    // ����ƽ̨
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

    // Ӧ������任
    if (camera) {
        camera->applyTransform();
    }

    // ��Ⱦ���β�
    for (int y = 0; y < tilemap.height; y++) {
        for (int x = 0; x < tilemap.width; x++) {
            if (y < tilemap.terrain.size() && x < tilemap.terrain[y].size() && tilemap.terrain[y][x] != 0) {
                draw_tile(&tilemap, tilemap.terrain[y][x], x * tilemap.tileSize, y * tilemap.tileSize);
            }
        }
    }

    // ��Ⱦװ�β�
    for (int y = 0; y < tilemap.height; y++) {
        for (int x = 0; x < tilemap.width; x++) {
            if (y < tilemap.decorations.size() && x < tilemap.decorations[y].size() && tilemap.decorations[y][x] != 0) {
                draw_tile(&tilemap, tilemap.decorations[y][x], x * tilemap.tileSize, y * tilemap.tileSize);
            }
        }
    }

    // ��������任
    if (camera) {
        camera->resetTransform();
    }
}

bool Level::checkWallCollision(const Vec2& position, const Vec2& size, float& depth, bool horizontal)
{
    // �����ƽ̨����ײ
    for (const auto& platform : platforms) {
        if (horizontal) {
            // ˮƽ��ײ���
            if (position.y + size.y > platform.position.y && 
                position.y < platform.position.y + platform.size.y) {
                if (position.x + size.x > platform.position.x && 
                    position.x < platform.position.x + platform.size.x) {
                    // ������ײ���
                    float leftDepth = platform.position.x + platform.size.x - position.x;
                    float rightDepth = position.x + size.x - platform.position.x;
                    depth = (leftDepth < rightDepth) ? -leftDepth : rightDepth;
                    return true;
                }
            }
        } else {
            // ��ֱ��ײ���
            if (position.x + size.x > platform.position.x && 
                position.x < platform.position.x + platform.size.x) {
                if (position.y + size.y > platform.position.y && 
                    position.y < platform.position.y + platform.size.y) {
                    // ������ײ���
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
    // ��tilemap�ĵ��β㴴��ƽ̨
    for (int y = 0; y < tilemap.height; y++) {
        for (int x = 0; x < tilemap.width; x++) {
            if (y < tilemap.terrain.size() && x < tilemap.terrain[y].size() && tilemap.terrain[y][x] != 0) {
                Vec2 pos(x * tilemap.tileSize, y * tilemap.tileSize);
                Vec2 size(tilemap.tileSize, tilemap.tileSize);
                platforms.emplace_back(pos, size);
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