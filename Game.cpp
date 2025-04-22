#include "Game.h"
#include "Input.h"

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
        player->update(currentLevel, dt);
        currentLevel->update(dt);
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