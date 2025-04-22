#pragma once

#include "assets.h"
#include "math.h"
#include "Camera.h"
#include "Tilemap.h"

// ����ͼ��
void draw_tile(const Tilemap* tilemap, int tileID, float x, float y);

// ��������任
void set_camera_transform(const Camera* camera);

// ��������任
void reset_camera_transform(); 