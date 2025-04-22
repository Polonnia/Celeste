#pragma once

#include "assets.h"
#include "math.h"
#include "Camera.h"
#include "Tilemap.h"

// 绘制图块
void draw_tile(const Tilemap* tilemap, int tileID, float x, float y);

// 设置相机变换
void set_camera_transform(const Camera* camera);

// 重置相机变换
void reset_camera_transform(); 