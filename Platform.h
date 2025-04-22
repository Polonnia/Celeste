#pragma once
#include "math.h"

class Platform {
public:
    Vec2 position;
    Vec2 size;

    Platform(const Vec2& pos, const Vec2& size)
        : position(pos)
        , size(size)
    {}
}; 