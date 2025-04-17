// 游戏主逻辑实现文件
#include "game.h"
#include "assets.h"

// #############################################################################
//                           Game Constants
// #############################################################################
// 游戏常量定义区域

// #############################################################################
//                           Game Structs
// #############################################################################
// 游戏结构体定义区域

// #############################################################################
//                           Game Functions
// #############################################################################
// 游戏核心功能实现区域
// 检查指定输入类型是否刚刚被按下
// @param type: 要检查的输入类型
// @return: 如果输入刚刚被按下返回true，否则返回false
bool just_pressed(GameInputType type)
{
  KeyMapping mapping = gameState->keyMappings[type];
  for(int idx = 0; idx < mapping.keys.count; idx++)
  {
    if(input->keys[mapping.keys[idx]].justPressed)
    {
      return true;
    }
  }

  return false;
}

// 检查指定输入类型是否处于按下状态
// @param type: 要检查的输入类型
// @return: 如果输入处于按下状态返回true，否则返回false
bool is_down(GameInputType type)
{
  KeyMapping mapping = gameState->keyMappings[type];
  for(int idx = 0; idx < mapping.keys.count; idx++)
  {
    if(input->keys[mapping.keys[idx]].isDown)
    {
      return true;
    }
  }

  return false;
}

// 将世界坐标转换为网格坐标
// @param worldPos: 世界坐标
// @return: 对应的网格坐标
IVec2 get_grid_pos(IVec2 worldPos)
{
  return {worldPos.x / TILESIZE, worldPos.y / TILESIZE};
}

// 获取指定网格位置的Tile
// @param x: 网格X坐标
// @param y: 网格Y坐标
// @return: 如果坐标有效返回对应的Tile指针，否则返回nullptr
Tile* get_tile(int x, int y)
{
  Tile* tile = nullptr;

  if(x >= 0  && x < WORLD_GRID.x && y >= 0 && y < WORLD_GRID.y)
  {
    tile = &gameState->worldGrid[x][y];
  }

  return tile;
}

// 获取指定世界坐标位置的Tile
// @param worldPos: 世界坐标
// @return: 如果坐标有效返回对应的Tile指针，否则返回nullptr
Tile* get_tile(IVec2 worldPos)
{
  IVec2 gridPos = get_grid_pos(worldPos);
  return get_tile(gridPos.x, gridPos.y);
}

// 获取玩家碰撞矩形
// @return: 玩家的碰撞矩形
IRect get_player_rect()
{  
  return 
  {
    gameState->player.pos.x - 4, 
    gameState->player.pos.y - 8, 
    8, 
    16
  };
}

// 获取指定网格位置的世界坐标
// @param x: 网格X坐标
// @param y: 网格Y坐标
// @return: 对应的世界坐标
IVec2 get_tile_pos(int x, int y)
{
  return {x * TILESIZE, y * TILESIZE};
}

// 获取指定网格位置的Tile矩形
// @param x: 网格X坐标
// @param y: 网格Y坐标
// @return: Tile的矩形区域
IRect get_tile_rect(int x, int y)
{
  return {get_tile_pos(x, y), 8, 8};
}

IRect get_solid_rect(Solid solid)
{
  Sprite sprite = get_sprite(solid.spriteID);
  return {solid.pos - sprite.size / 2, sprite.size};
}

// 更新游戏关卡状态
// @param dt: 帧间隔时间
void update_level(float dt)
{
  // 检查是否按下暂停键
  if(just_pressed(PAUSE))
  {
    gameState->state = GAME_STATE_MAIN_MENU;
  }

  // 更新玩家状态
  {
    Player& player = gameState->player;
    player.prevPos = player.pos;
    player.animationState = PLAYER_ANIM_IDLE;

    static Vec2 remainder = {};
    static bool grounded = false;
    constexpr float runSpeed = 2.0f;
    constexpr float runAcceleration = 10.0f;
    constexpr float runReduce = 22.0f; 
    constexpr float flyReduce = 12.0f;    
    constexpr float gravity = 13.0f;
    constexpr float fallSpeed = 3.6f;
    constexpr float jumpSpeed = -3.0f;

    // Facing the Player in the right direction
    if(player.speed.x > 0)
    {
      player.renderOptions = 0;
    }
    if(player.speed.x < 0)
    {
      player.renderOptions = RENDER_OPTION_FLIP_X;
    }

    // Jump
    if(just_pressed(JUMP) && grounded)
    {
      player.speed.y = jumpSpeed;
      player.speed.x += player.solidSpeed.x;
      player.speed.y += player.solidSpeed.y;
      play_sound("jump");
      grounded = false;
    }

    if(!grounded)
    {
      player.animationState = PLAYER_ANIM_JUMP;
    }

    if(is_down(MOVE_LEFT))
    {
      if(grounded)
      {
        player.animationState = PLAYER_ANIM_RUN;
      }

      float mult = 1.0f;
      if(player.speed.x > 0.0f)
      {
        mult = 3.0f;
      }
      player.runAnimTime += dt;
      player.speed.x = approach(player.speed.x, -runSpeed, runAcceleration * mult * dt);
    }

    if(is_down(MOVE_RIGHT))
    {
      if(grounded)
      {
        player.animationState = PLAYER_ANIM_RUN;
      }

      float mult = 1.0f;
      if(player.speed.x < 0.0f)
      {
        mult = 3.0f;
      }
      player.runAnimTime += dt;
      player.speed.x = approach(player.speed.x, runSpeed, runAcceleration * mult * dt);
    }

    // Friction
    if(!is_down(MOVE_LEFT) &&
      !is_down(MOVE_RIGHT))
    {
      if(grounded)
      {
        player.speed.x = approach(player.speed.x, 0, runReduce * dt);
      }
      else
      {
        player.speed.x = approach(player.speed.x, 0, flyReduce * dt);
      }
    }

    // Gravity
    player.speed.y = approach(player.speed.y, fallSpeed, gravity * dt);


    if(is_down(MOVE_UP))
    {
      player.pos = {};
    }

    // Move X
    {
      IRect playerRect = get_player_rect();

      remainder.x += player.speed.x;
      int moveX = round(remainder.x);
      if(moveX != 0)
      {
        remainder.x -= moveX;
        int moveSign = sign(moveX);
        bool collisionHappened = false;

        // Move the player in Y until collision or moveY is exausted
        auto movePlayerX = [&]
        {
          while(moveX)
          {
            playerRect.pos.x += moveSign;

            // Test collision against Solids
            {
              for(int solidIdx = 0; solidIdx < gameState->solids.count; solidIdx++)
              {
                Solid& solid = gameState->solids[solidIdx];
                IRect solidRect = get_solid_rect(solid);

                if(rect_collision(playerRect, solidRect))
                {
                  player.speed.x = 0;
                  return;
                }
              }
            }

            // Loop through local Tiles
            IVec2 playerGridPos = get_grid_pos(player.pos);
            for(int x = playerGridPos.x - 1; x <= playerGridPos.x + 1; x++)
            {
              for(int y = playerGridPos.y - 2; y <= playerGridPos.y + 2; y++)
              {
                Tile* tile = get_tile(x, y);

                if(!tile || !tile->isVisible)
                {
                  continue;
                }

                IRect tileRect = get_tile_rect(x, y);
                if(rect_collision(playerRect, tileRect))
                {
                  player.speed.x = 0;
                  return;
                }
              }
            }

            // Move the Player
            player.pos.x += moveSign;
            moveX -= moveSign;
          }
        };
        movePlayerX();
      }
    }

    // Move Y
    {
      IRect playerRect = get_player_rect();

      remainder.y += player.speed.y;
      int moveY = round(remainder.y);
      if(moveY != 0)
      {
        remainder.y -= moveY;
        int moveSign = sign(moveY);
        bool collisionHappened = false;

        // Move the player in Y until collision or moveY is exausted
        auto movePlayerY = [&]
        {
          while(moveY)
          {
            playerRect.pos.y += moveSign;

            // Test collision against Solids
            {
              for(int solidIdx = 0; solidIdx < gameState->solids.count; solidIdx++)
              {
                Solid& solid = gameState->solids[solidIdx];
                IRect solidRect = get_solid_rect(solid);

                if(rect_collision(playerRect, solidRect))
                {
                  // Moving down/falling
                  if(player.speed.y > 0.0f)
                  {
                    grounded = true;
                  }

                  player.speed.y = 0;
                  return;
                }
              }
            }

            // Loop through local Tiles
            IVec2 playerGridPos = get_grid_pos(player.pos);
            for(int x = playerGridPos.x - 1; x <= playerGridPos.x + 1; x++)
            {
              for(int y = playerGridPos.y - 2; y <= playerGridPos.y + 2; y++)
              {
                Tile* tile = get_tile(x, y);

                if(!tile || !tile->isVisible)
                {
                  continue;
                }

                IRect tileRect = get_tile_rect(x, y);
                if(rect_collision(playerRect, tileRect))
                {
                  // Moving down/falling
                  if(player.speed.y > 0.0f)
                  {
                    grounded = true;
                  }

                  player.speed.y = 0;
                  return;
                }
              }
            }

            // Move the Player
            player.pos.y += moveSign;
            moveY -= moveSign;
          }
        };
        movePlayerY();
      }
    }
  }

  // 更新固体对象
  {
    Player& player = gameState->player;
    player.solidSpeed = {};

    for(int solidIdx = 0; solidIdx < gameState->solids.count; solidIdx++)
    {
      Solid& solid = gameState->solids[solidIdx];
      solid.prevPos = solid.pos;

      IRect solidRect = get_solid_rect(solid);
      solidRect.pos -= 1;
      solidRect.size += 2;

      int nextKeyframeIdx = solid.keyframeIdx + 1;
      nextKeyframeIdx %= solid.keyframes.count;

      // Move X
      {
        solid.remainder.x += solid.speed.x * dt;
        int moveX = round(solid.remainder.x);
        if(moveX != 0)
        {
          solid.remainder.x -= moveX;
          int moveSign = sign(solid.keyframes[nextKeyframeIdx].x - 
                              solid.keyframes[solid.keyframeIdx].x);

          // Move the player in Y until collision or moveY is exausted
          auto moveSolidX = [&]
          {
            while(moveX)
            {
              IRect playerRect = get_player_rect();
              bool standingOnTop = 
                playerRect.pos.y - 1 + playerRect.size.y == solidRect.pos.y;

              solidRect.pos.x += moveSign;

              // Collision happend on left or right, push the player
              bool tileCollision = false;
              if(rect_collision(playerRect, solidRect))
              {
                // Move the player rect
                playerRect.pos.x += moveSign;
                player.solidSpeed.x = solid.speed.x * (float)moveSign / 20.0f;

                // Check for collision, if yes, destroy the player
                // Loop through local Tiles
                IVec2 playerGridPos = get_grid_pos(player.pos);
                for(int x = playerGridPos.x - 1; x <= playerGridPos.x + 1; x++)
                {
                  for(int y = playerGridPos.y - 2; y <= playerGridPos.y + 2; y++)
                  {
                    Tile* tile = get_tile(x, y);

                    if(!tile || !tile->isVisible)
                    {
                      continue;
                    }

                    IRect tileRect = get_tile_rect(x, y);
                    if(rect_collision(playerRect, tileRect))
                    {
                      tileCollision = true;

                      if(!standingOnTop)
                      {
                        // Death
                      player.pos = {WORLD_WIDTH / 2,  WORLD_HEIGHT - 24};
                      }
                    }
                  }
                }

                if(!tileCollision)
                {
                  // Actually move the player
                  player.pos.x += moveSign;
                }
              }

              // Move the Solid
              solid.pos.x += moveSign;
              moveX -= 1;

              if(solid.pos.x == solid.keyframes[nextKeyframeIdx].x)
              {
                solid.keyframeIdx = nextKeyframeIdx;
                nextKeyframeIdx++;
                nextKeyframeIdx %= solid.keyframes.count;
              }
            }
          };
          moveSolidX();
        }
      }

      // Move Y
      {
        solid.remainder.y += solid.speed.y * dt;
        int moveY = round(solid.remainder.y);
        if(moveY != 0)
        {
          solid.remainder.y -= moveY;
          int moveSign = sign(solid.keyframes[nextKeyframeIdx].y - 
                              solid.keyframes[solid.keyframeIdx].y);

          // Move the player in Y until collision or moveY is exausted
          auto moveSolidY = [&]
          {
            while(moveY)
            {
              IRect playerRect = get_player_rect();
              solidRect.pos.x += moveSign;

              // Collision happend on bottom, push the player
              if(rect_collision(playerRect, solidRect))
              {
                // Move the player
                player.pos.y += moveSign;
                player.solidSpeed.y = solid.speed.y * (float)moveSign / 40.0f;

                // Check for collision, if yes, destroy the player
                // Loop through local Tiles
                IVec2 playerGridPos = get_grid_pos(player.pos);
                for(int x = playerGridPos.x - 1; x <= playerGridPos.x + 1; x++)
                {
                  for(int y = playerGridPos.y - 2; y <= playerGridPos.y + 2; y++)
                  {
                    Tile* tile = get_tile(x, y);

                    if(!tile || !tile->isVisible)
                    {
                      continue;
                    }

                    IRect tileRect = get_tile_rect(x, y);
                    if(rect_collision(playerRect, tileRect))
                    {
                      player.pos = {WORLD_WIDTH / 2,  WORLD_HEIGHT - 24};
                    }
                  }
                }
              }

              // Move the Solid
              solid.pos.y += moveSign;
              moveY -= 1;

              if(solid.pos.y == solid.keyframes[nextKeyframeIdx].y)
              {
                solid.keyframeIdx = nextKeyframeIdx;
                nextKeyframeIdx++;
                nextKeyframeIdx %= solid.keyframes.count;
              }
            }
          };
          moveSolidY();
        }
      }
    }
  }

  // 处理Tile编辑
  bool updateTiles = false;
  if(is_down(MOUSE_LEFT) && !ui_is_hot() && !ui_is_active())
  {
    // 左键点击使Tile可见
    IVec2 worldPos = screen_to_world(input->mousePos);
    Tile* tile = get_tile(worldPos);
    if(tile)
    {
      tile->isVisible = true;
      updateTiles = true;
    }
  }

  if(is_down(MOUSE_RIGHT))
  {
    // 右键点击使Tile不可见
    IVec2 worldPos = screen_to_world(input->mousePos);
    Tile* tile = get_tile(worldPos);
    if(tile)
    {
      tile->isVisible = false;
      updateTiles = true;
    }
  }

  // 更新Tile的邻居掩码
  if(updateTiles)
  {
    // Neighbouring Tiles        Top    Left      Right       Bottom  
    int neighbourOffsets[24] = { 0,-1,  -1, 0,     1, 0,       0, 1,   
    //                          Topleft Topright Bottomleft Bottomright
                                -1,-1,   1,-1,    -1, 1,       1, 1,
    //                           Top2   Left2     Right2      Bottom2
                                 0,-2,  -2, 0,     2, 0,       0, 2};

    // Topleft     = BIT(4) = 16
    // Toplright   = BIT(5) = 32
    // Bottomleft  = BIT(6) = 64
    // Bottomright = BIT(7) = 128

    for(int y = 0; y < WORLD_GRID.y; y++)
    {
      for(int x = 0; x < WORLD_GRID.x; x++)
      {
        Tile* tile = get_tile(x, y);

        if(!tile->isVisible)
        {
          continue;
        }

        tile->neighbourMask = 0;
        int neighbourCount = 0;
        int extendedNeighbourCount = 0;
        int emptyNeighbourSlot = 0;

        // Look at the sorrounding 12 Neighbours
        for(int n = 0; n < 12; n++)
        {
          Tile* neighbour = get_tile(x + neighbourOffsets[n * 2],
                                     y + neighbourOffsets[n * 2 + 1]);

          // No neighbour means the edge of the world
          if(!neighbour || neighbour->isVisible)
          {
            tile->neighbourMask |= BIT(n);
            if(n < 8) // Counting direct neighbours
            {
              neighbourCount++;
            }
            else // Counting neighbours 1 Tile away
            {
              extendedNeighbourCount++;
            }
          }
          else if(n < 8)
          {
            emptyNeighbourSlot = n;
          }
        }

        if(neighbourCount == 7 && emptyNeighbourSlot >= 4) // We have a corner
        {
          tile->neighbourMask = 16 + (emptyNeighbourSlot - 4);
        }
        else if(neighbourCount == 8 && extendedNeighbourCount == 4)
        {
          tile->neighbourMask = 20;
        }
        else
        {
          tile->neighbourMask = tile->neighbourMask & 0b1111;
        }
      }
    }
  }
}

// 更新主菜单状态
// @param dt: 帧间隔时间
void update_main_menu(float dt)
{
  // 创建开始按钮
  int buttonID = line_id(1);
  Vec4 color = COLOR_WHITE;

  // 鼠标悬停时改变按钮颜色
  if(is_hot(buttonID))
  {
    color = COLOR_GREEN;
  }

  // 绘制开始按钮并处理点击事件
  if(do_button(SPRITE_BUTTON_PLAY, IVec2{WORLD_WIDTH/2, WORLD_HEIGHT/2}, 
               buttonID, {.material{.color = color}, .layer = get_layer(LAYER_UI, 10.0f)}))
  {
    // 点击按钮后进入游戏关卡
    gameState->state = GAME_STATE_IN_LEVEL;
  }

  // 绘制游戏标题
  do_ui_text("Celeste Clone", {56, 20}, 
             {.material{.color = COLOR_BLACK}, 
             .fontSize = 2.0f, 
             .layer = get_layer(LAYER_UI, 10)});

  // 绘制菜单背景
  do_ui_quad(
    {(float)WORLD_WIDTH / 2, (float)WORLD_HEIGHT / 2},
    {(float)WORLD_WIDTH, (float)WORLD_HEIGHT},
    {      
      .material{.color = {79.0f / 255.0f, 140.0f / 255.0f, 235.0f / 255.0f, 1.0f}},
      .layer = get_layer(LAYER_UI, 0.0f)
    });
}

// 游戏主循环更新函数
void simulate()
{
  float dt = UPDATE_DELAY;

  // 根据当前游戏状态调用相应的更新函数
  switch(gameState->state)
  {
    case GAME_STATE_IN_LEVEL:
    {
      update_level(dt);
      break;
    }
  
    case GAME_STATE_MAIN_MENU:
    {
      update_main_menu(dt);
      break;
    }
  }
}

// #############################################################################
//                           Game Functions(exposed)
// #############################################################################
EXPORT_FN void update_game(GameState* gameStateIn, 
                         RenderData* renderDataIn, 
                         Input* inputIn, 
                         SoundState* soundStateIn,
                         UIState* uiStateIn,
                         float dt)
{
  // 初始化全局状态
  if(renderData != renderDataIn)
  {
    gameState = gameStateIn;
    renderData = renderDataIn;
    input = inputIn;
    soundState = soundStateIn;
    uiState = uiStateIn;
  }

  // 初始化游戏状态
  if(!gameState->initialized)
  {
    // 设置相机参数
    renderData->gameCamera.dimensions = {WORLD_WIDTH, WORLD_HEIGHT};
    renderData->gameCamera.position.x = 160;
    renderData->gameCamera.position.y = -90;

    renderData->uiCamera.dimensions = {WORLD_WIDTH, WORLD_HEIGHT};
    renderData->uiCamera.position.x = 160;
    renderData->uiCamera.position.y = -90;

    // 初始化玩家动画精灵
    {
      Player& player = gameState->player;
      player.animationSprites[PLAYER_ANIM_IDLE] = SPRITE_CELESTE;
      player.animationSprites[PLAYER_ANIM_JUMP] = SPRITE_CELESTE_JUMP;
      player.animationSprites[PLAYER_ANIM_RUN] = SPRITE_CELESTE_RUN;
    }

    // 初始化Tile集
    {
      IVec2 tilesPosition = {48, 0};

      for(int y = 0; y < 5; y++)
      {
        for(int x = 0; x < 4; x++)
        {
          gameState->tileCoords.add({tilesPosition.x +  x * 8, tilesPosition.y + y * 8});
        }
      }

      // 添加黑色内部Tile
      gameState->tileCoords.add({tilesPosition.x, tilesPosition.y + 5 * 8});
    }

    // 设置按键映射
    {
      gameState->keyMappings[MOVE_UP].keys.add(KEY_W);
      gameState->keyMappings[MOVE_UP].keys.add(KEY_UP);
      gameState->keyMappings[MOVE_LEFT].keys.add(KEY_A);
      gameState->keyMappings[MOVE_LEFT].keys.add(KEY_LEFT);
      gameState->keyMappings[MOVE_DOWN].keys.add(KEY_S);
      gameState->keyMappings[MOVE_DOWN].keys.add(KEY_DOWN);
      gameState->keyMappings[MOVE_RIGHT].keys.add(KEY_D);
      gameState->keyMappings[MOVE_RIGHT].keys.add(KEY_RIGHT);
      gameState->keyMappings[MOUSE_LEFT].keys.add(KEY_MOUSE_LEFT);
      gameState->keyMappings[MOUSE_RIGHT].keys.add(KEY_MOUSE_RIGHT);
      gameState->keyMappings[JUMP].keys.add(KEY_SPACE);
      gameState->keyMappings[PAUSE].keys.add(KEY_ESCAPE);
    }

    // 初始化固体对象
    {
      Solid solid = {};
      solid.spriteID = SPRITE_SOLID_01;
      solid.keyframes.add({8 * 2,  8 * 10});
      solid.keyframes.add({8 * 10, 8 * 10});
      solid.pos = {8 * 2, 8 * 10};
      solid.speed.x = 50.0f;
      gameState->solids.add(solid);

      solid = {};
      solid.spriteID = SPRITE_SOLID_02;
      solid.keyframes.add({12 * 20, 8 * 10});
      solid.keyframes.add({12 * 20, 8 * 20});
      solid.pos = {12 * 20, 8 * 10};
      solid.speed.y = 50.0f;
      gameState->solids.add(solid);
    }

    gameState->initialized = true;
  }

  // 固定更新循环
  {
    gameState->updateTimer += dt;
    while(gameState->updateTimer >= UPDATE_DELAY)
    {
      gameState->updateTimer -= UPDATE_DELAY;
      update_ui();
      simulate();

      // 更新鼠标相对位置
      input->relMouse = input->mousePos - input->prevMousePos;
      input->prevMousePos = input->mousePos;

      // 清除所有按键的过渡状态
      {
        for (int keyCode = 0; keyCode < KEY_COUNT; keyCode++)
        {
          input->keys[keyCode].justReleased = false;
          input->keys[keyCode].justPressed = false;
          input->keys[keyCode].halfTransitionCount = 0;
        }
      }
    }
  }

  float interpolatedDT = (float)(gameState->updateTimer / UPDATE_DELAY);

  // 绘制UI元素
  {
    for(int uiElementIdx = 0; uiElementIdx < uiState->uiElements.count; uiElementIdx++)
    {
      UIElement& uiElement = uiState->uiElements[uiElementIdx];
      draw_ui_sprite(uiElement.spriteID, uiElement.pos, uiElement.size, uiElement.drawData);
    }

    for(int uiTextIdx = 0; uiTextIdx < uiState->uiTexts.count; uiTextIdx++)
    {
      UIText& uiText = uiState->uiTexts[uiTextIdx];
      draw_ui_text(uiText.text, uiText.pos, uiText.textData);
    }
  }

  // 绘制固体对象
  {
    for(int solidIdx = 0; solidIdx < gameState->solids.count; solidIdx++)
    {
      Solid& solid = gameState->solids[solidIdx];
      IVec2 solidPos = lerp(solid.prevPos, solid.pos, interpolatedDT);
      draw_sprite(solid.spriteID, solidPos, {.layer = get_layer(LAYER_GAME, 0)});
    }
  }

  // 绘制玩家
  {
    Player& player = gameState->player;
    IVec2 playerPos = lerp(player.prevPos, player.pos, interpolatedDT);
    
    Sprite sprite = get_sprite(player.animationSprites[player.animationState]);
    int animationIdx = animate(&player.runAnimTime, sprite.frameCount, 0.6f);
    draw_sprite(player.animationSprites[player.animationState], playerPos, 
                {
                  .animationIdx = animationIdx,
                  .renderOptions = player.renderOptions,
                  .layer = get_layer(LAYER_GAME, 0)
                });
  }

  // 绘制Tile集
  {
    for(int y = 0; y < WORLD_GRID.y; y++)
    {
      for(int x = 0; x < WORLD_GRID.x; x++)
      {
        Tile* tile = get_tile(x, y);

        if(!tile->isVisible)
        {
          continue;
        }

        // 绘制Tile
        Transform transform = {};
        transform.materialIdx = get_material_idx({.color  = COLOR_WHITE});
        transform.pos = {x * (float)TILESIZE, y * (float)TILESIZE};
        transform.size = {8, 8};
        transform.spriteSize = {8, 8};
        transform.atlasOffset = gameState->tileCoords[tile->neighbourMask];
        transform.layer = get_layer(LAYER_GAME, 0);
        draw_quad(transform);
      }
    }
  }
}