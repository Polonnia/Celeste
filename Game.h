#pragma once
#include "Level.h"
#include "Player.h"

class Game {
public:
    Game();
    ~Game();

    void init();
    void update(float dt);
    void render();
    void handleInput();

private:
    Level* currentLevel;
    Player* player;
}; 