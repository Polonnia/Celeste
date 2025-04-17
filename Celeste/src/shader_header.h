#pragma once

#include "schnitzel_lib.h"

// 定义着色器中的类型别名，使其与C++代码中的类型对应
#define vec2 Vec2    // 二维浮点向量
#define ivec2 IVec2  // 二维整型向量
#define vec4 Vec4    // 四维浮点向量

// 定义位操作宏
#define BIT(i) 1 << i  // 将1左移i位，用于位标志操作

// #############################################################################
//                           Rendering Constants
// #############################################################################
// 渲染选项常量定义
int RENDERING_OPTION_FLIP_X = BIT(0);  // 水平翻转渲染选项
int RENDERING_OPTION_FLIP_Y = BIT(1);  // 垂直翻转渲染选项
int RENDERING_OPTION_FONT = BIT(2);    // 字体渲染选项

// #############################################################################
//                           Rendering Structs
// #############################################################################
// 变换结构体，用于描述渲染对象的空间变换
struct Transform
{
  Vec2 pos;           // 位置（当前是左上角坐标）
  Vec2 size;          // 大小
  IVec2 atlasOffset;  // 纹理图集偏移
  IVec2 spriteSize;   // 精灵大小
  int renderOptions;  // 渲染选项
  int materialIdx;    // 材质索引
  float layer;        // 渲染层级
  int padding;        // 填充字节
};

// 材质结构体，用于描述渲染对象的材质属性
struct Material
{
  Vec4 color = COLOR_WHITE;  // 颜色，默认为白色
  
  // 重载相等运算符，用于比较两个材质是否相同
  bool operator==(const Material& other) const
  {
    return color.x == other.color.x && 
           color.y == other.color.y && 
           color.z == other.color.z && 
           color.w == other.color.w;
  }
};