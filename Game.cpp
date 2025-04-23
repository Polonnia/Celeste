#include "Game.h"
#include "Input.h"
#include "Tilemap.h"

Game::Game()
    : currentLevel(nullptr)
    , player(nullptr)
{
}

Game::~Game()
{
    delete currentLevel;
    delete player;
}

void Game::init()
{
    // 创建并加载第一关
    currentLevel = new Level("level1");
    
    // 创建玩家，设置初始位置
    player = new Player(100.0f, 100.0f);
}

void Game::handleInput()
{
    if (player) {
        player->handleInput();
    }
}

void Game::update(float dt)
{
    if (player && currentLevel) {
        // 保存玩家上一帧的位置
        float prevX = player->getPosition().x;
        
        // 更新玩家和关卡
        player->update(currentLevel, dt);
        currentLevel->update(dt);
        
        // 计算玩家移动距离
        float deltaX = player->getPosition().x - prevX;
        
        // 更新后景图片位置
        if (currentLevel->getTilemap()) {
            update_backgrounds(currentLevel->getTilemap(), player->getPosition().x, deltaX);
        }
    }
}

void Game::render()
{
    if (currentLevel) {
        currentLevel->render();
    }
    
    if (player) {
        player->render();
    }
} 