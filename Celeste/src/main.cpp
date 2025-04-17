#include "schnitzel_lib.h"

#include "input.h"

#include "game.h"

#include "sound.h"

#include "ui.h"

#include "renderer.h"


// #############################################################################
//                           Game DLL Stuff
// #############################################################################
// This is the function pointer to update_game in game.cpp
typedef decltype(update_game) update_game_type;
static update_game_type* update_game_ptr;

// #############################################################################
//                           Cross Platform functions
// #############################################################################
// Used to get Delta Time
#include <chrono>
double get_delta_time();
void reload_game_dll(BumpAllocator* transientStorage);

// #############################################################################
//                           Game Globals
// #############################################################################
// 游戏全局变量定义区域

// #############################################################################
//                           Game Functions
// #############################################################################
// 游戏核心功能实现区域

// 游戏初始化函数
// @param bumpAllocator: 内存分配器
void init_game(BumpAllocator* bumpAllocator)
{
  // 初始化游戏状态
  gameState = (GameState*)bump_alloc(bumpAllocator, sizeof(GameState));
  gameState->state = GAME_STATE_MAIN_MENU;
  gameState->updateTimer = 0.0f;

  // 初始化玩家
  Player& player = gameState->player;
  player.pos = {WORLD_WIDTH / 2, WORLD_HEIGHT / 2};
  player.prevPos = player.pos;
  player.speed = {};
  player.solidSpeed = {};
  player.renderOptions = 0;
  player.runAnimTime = 0.0f;
  player.animationState = PLAYER_ANIM_IDLE;
  player.animationSprites[PLAYER_ANIM_IDLE] = SPRITE_CELESTE;
  player.animationSprites[PLAYER_ANIM_RUN] = SPRITE_CELESTE_RUN;
  player.animationSprites[PLAYER_ANIM_JUMP] = SPRITE_CELESTE_JUMP;

  // 初始化Tile坐标
  gameState->tileCoords.add({0, 0});    // 0
  gameState->tileCoords.add({8, 0});    // 1
  gameState->tileCoords.add({16, 0});   // 2
  gameState->tileCoords.add({24, 0});   // 3
  gameState->tileCoords.add({32, 0});   // 4
  gameState->tileCoords.add({40, 0});   // 5
  gameState->tileCoords.add({48, 0});   // 6
  gameState->tileCoords.add({56, 0});   // 7
  gameState->tileCoords.add({64, 0});   // 8
  gameState->tileCoords.add({72, 0});   // 9
  gameState->tileCoords.add({80, 0});   // 10
  gameState->tileCoords.add({88, 0});   // 11
  gameState->tileCoords.add({96, 0});   // 12
  gameState->tileCoords.add({104, 0});  // 13
  gameState->tileCoords.add({112, 0});  // 14
  gameState->tileCoords.add({120, 0});  // 15
  gameState->tileCoords.add({128, 0});  // 16
  gameState->tileCoords.add({136, 0});  // 17
  gameState->tileCoords.add({144, 0});  // 18
  gameState->tileCoords.add({152, 0});  // 19
  gameState->tileCoords.add({160, 0});  // 20

  // 初始化按键映射
  KeyMapping& left = gameState->keyMappings[MOVE_LEFT];
  left.keys.add(KEY_A);
  left.keys.add(KEY_LEFT);

  KeyMapping& right = gameState->keyMappings[MOVE_RIGHT];
  right.keys.add(KEY_D);
  right.keys.add(KEY_RIGHT);

  KeyMapping& up = gameState->keyMappings[MOVE_UP];
  up.keys.add(KEY_W);
  up.keys.add(KEY_UP);

  KeyMapping& down = gameState->keyMappings[MOVE_DOWN];
  down.keys.add(KEY_S);
  down.keys.add(KEY_DOWN);

  KeyMapping& jump = gameState->keyMappings[JUMP];
  jump.keys.add(KEY_SPACE);
  jump.keys.add(KEY_Z);

  KeyMapping& mouseLeft = gameState->keyMappings[MOUSE_LEFT];
  mouseLeft.keys.add(KEY_MOUSE_LEFT);

  KeyMapping& mouseRight = gameState->keyMappings[MOUSE_RIGHT];
  mouseRight.keys.add(KEY_MOUSE_RIGHT);

  KeyMapping& pause = gameState->keyMappings[PAUSE];
  pause.keys.add(KEY_ESCAPE);
  pause.keys.add(KEY_P);

  // 初始化UI状态
  uiState = (UIState*)bump_alloc(bumpAllocator, sizeof(UIState));
}

// 游戏主循环
// @param bumpAllocator: 内存分配器
void game_loop(BumpAllocator* bumpAllocator)
{
  // 初始化游戏
  init_game(bumpAllocator);

  // 初始化渲染器
  if(!easyx_init())
  {
    SM_ASSERT(false, "Failed to initialize renderer!");
    return;
  }

  // 初始化音频
  if(!platform_init_audio())
  {
    SM_ASSERT(false, "Failed to initialize audio!");
    return;
  }

  // 主循环
  while(running)
  {
    // 更新窗口状态
    platform_update_window();

    // 更新游戏状态
    simulate();

    // 渲染游戏画面
    easyx_render();

    // 更新音频
    platform_update_audio(UPDATE_DELAY);

    // 控制帧率
    platform_sleep(16);
  }

  // 清理资源
  easyx_cleanup();
}

// 程序入口点
int main(int argc, char* argv[])
{
  // 创建内存分配器
  BumpAllocator persistentStorage = make_bump_allocator(MB(5));

  // 初始化输入系统
  input = (Input*)bump_alloc(&persistentStorage, sizeof(Input));
  input->screenSize = {1280, 720};

  // 初始化渲染数据
  renderData = (RenderData*)bump_alloc(&persistentStorage, sizeof(RenderData));

  // 填充按键映射表
  platform_fill_keycode_lookup_table();

  // 创建游戏窗口
  if(!platform_create_window(input->screenSize.x, input->screenSize.y, "Celeste Clone"))
  {
    SM_ASSERT(false, "Failed to create window!");
    return -1;
  }

  // 设置垂直同步
  platform_set_vsync(true);

  // 启动游戏主循环
  game_loop(&persistentStorage);

  return 0;
}

void update_game(GameState* gameStateIn, 
                RenderData* renderDataIn, 
                Input* inputIn,
                SoundState* soundStateIn,
                UIState* uiStateIn,
                float dt)
{
  update_game_ptr(gameStateIn ,renderDataIn, inputIn, soundStateIn, uiStateIn, dt);
}

double get_delta_time()
{
  // Only executed once when entering the function (static)
  static auto lastTime = std::chrono::steady_clock::now();
  auto currentTime = std::chrono::steady_clock::now();

  // seconds
  double delta = std::chrono::duration<double>(currentTime - lastTime).count(); 
  lastTime = currentTime; 

  return delta;
}

void reload_game_dll(BumpAllocator* transientStorage)
{
  static void* gameDLL;
  static long long lastEditTimestampGameDLL;

  long long currentTimestampGameDLL = get_timestamp(gameLibName);
  if(currentTimestampGameDLL > lastEditTimestampGameDLL)
  {
    if(gameDLL)
    {
      bool freeResult = platform_free_dynamic_library(gameDLL);
      SM_ASSERT(freeResult, "Failed to free %s", gameLibName);
      gameDLL = nullptr;
      SM_TRACE("Freed %s", gameLibName);
    }

    while(!copy_file(gameLibName, gameLoadLibName, transientStorage))
    {
      platform_sleep(10);
    }
    SM_TRACE("Copied %s into %s", gameLibName, gameLoadLibName);

    gameDLL = platform_load_dynamic_library(gameLoadLibName);
    SM_ASSERT(gameDLL, "Failed to load %s", gameLoadLibName);

    update_game_ptr = (update_game_type*)platform_load_dynamic_function(gameDLL, "update_game");
    SM_ASSERT(update_game_ptr, "Failed to load update_game function");
    lastEditTimestampGameDLL = currentTimestampGameDLL;
  }
}




