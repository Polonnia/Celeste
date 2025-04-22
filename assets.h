#pragma once

#include "math.h"

// #############################################################################
//                           Assets Constants
// #############################################################################

// #############################################################################
//                           Assets Structs
// #############################################################################

enum SpriteID
{
  SPRITE_WHITE,
  SPRITE_DICE,
  SPRITE_CELESTE,
  SPRITE_CELESTE_RUN,
  SPRITE_CELESTE_JUMP,
  SPRITE_SOLID_01,
  SPRITE_SOLID_02,  
  SPRITE_BUTTON_PLAY,
  SPRITE_BUTTON_SAVE,

  SPRITE_COUNT
};

struct SpriteData
{
  Vec2 atlasOffset;
  Vec2 size;
  int frames;
  float frameTime;
  int frameCount;
};

// #############################################################################
//                           Assets Functions
// #############################################################################
SpriteData get_sprite(SpriteID spriteID);
