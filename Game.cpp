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
    // ���������ص�һ��
    currentLevel = new Level("level1");
    
    // ������ң����ó�ʼλ��
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