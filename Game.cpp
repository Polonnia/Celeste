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
        // ���������һ֡��λ��
        float prevX = player->getPosition().x;
        
        // ������Һ͹ؿ�
        player->update(currentLevel, dt);
        currentLevel->update(dt);
        
        // ��������ƶ�����
        float deltaX = player->getPosition().x - prevX;
        
        // ���º�ͼƬλ��
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