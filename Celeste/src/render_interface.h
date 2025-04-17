#pragma once

#include "assets.h"
#include "shader_header.h"
#include "schnitzel_lib.h"


// #############################################################################
//                           Renderer Constants
// #############################################################################
// 渲染选项常量定义
int RENDER_OPTION_FLIP_X = BIT(0);  // 水平翻转渲染选项
int RENDER_OPTION_FLIP_Y = BIT(1);  // 垂直翻转渲染选项

// #############################################################################
//                           Renderer Structs
// #############################################################################
// 渲染层级枚举
enum Layer
{
  LAYER_GAME,    // 游戏层，用于渲染游戏对象
  LAYER_UI,      // UI层，用于渲染用户界面
  LAYER_COUNT    // 层级总数
};

// 2D正交相机结构体
struct OrthographicCamera2D
{
  float zoom = 1.0f;        // 缩放比例
  Vec2 dimensions;          // 相机尺寸
  Vec2 position;            // 相机位置
};

// 绘制数据结构体
struct DrawData
{
  Material material = {};    // 材质信息
  int animationIdx;         // 动画帧索引
  int renderOptions;        // 渲染选项
  float layer = 0.0f;       // 渲染层级
};

// 文本数据结构体
struct TextData
{
  Material material = {};    // 材质信息
  float fontSize = 1.0f;    // 字体大小
  int renderOptions;        // 渲染选项
  float layer = 0.0f;       // 渲染层级
};

// 字形结构体
struct Glyph
{
  Vec2 offset;              // 字形偏移量
  Vec2 advance;             // 字形前进量
  IVec2 textureCoords;      // 纹理坐标
  IVec2 size;               // 字形大小
};

// 渲染数据结构体
struct RenderData
{
  OrthographicCamera2D gameCamera;  // 游戏相机
  OrthographicCamera2D uiCamera;    // UI相机

  int fontHeight;                   // 字体高度
  Glyph glyphs[127];                // 字形数组

  Array<Material, 1000> materials;  // 材质数组
  Array<Transform, 1000> transforms; // 变换数组
  Array<Transform, 1000> uiTransforms; // UI变换数组
};

// #############################################################################
//                           Renderer Globals
// #############################################################################
static RenderData* renderData;  // 全局渲染数据指针

// #############################################################################
//                           Renderer Untility
// #############################################################################
// 将屏幕坐标转换为世界坐标
// @param screenPos: 屏幕坐标
// @return: 对应的世界坐标
IVec2 screen_to_world(IVec2 screenPos)
{
  OrthographicCamera2D camera = renderData->gameCamera;

  // 计算X坐标
  int xPos = (float)screenPos.x / 
             (float)input->screenSize.x * 
             camera.dimensions.x; // [0; dimensions.x]

  // 使用尺寸和位置进行偏移
  xPos += -camera.dimensions.x / 2.0f + camera.position.x;

  // 计算Y坐标
  int yPos = (float)screenPos.y / 
             (float)input->screenSize.y * 
             camera.dimensions.y; // [0; dimensions.y]

  // 使用尺寸和位置进行偏移
  yPos += camera.dimensions.y / 2.0f + camera.position.y;

  return {xPos, yPos};
}

// 计算动画帧索引
// @param time: 动画时间
// @param frameCount: 总帧数
// @param duration: 动画持续时间
// @return: 当前帧索引
int animate(float* time, int frameCount, float duration = 1.0f)
{
  // 循环动画时间
  while(*time > duration)
  {
    *time -= duration;
  }
  
  // 计算当前帧索引
  int animationIdx = (int)((*time / duration) * frameCount);
  
  // 确保索引在有效范围内
  if (animationIdx >= frameCount)
  {
    animationIdx = frameCount - 1;
  }
  
  return animationIdx;
}

// 获取材质索引
// @param material: 材质信息
// @return: 材质在数组中的索引
int get_material_idx(Material material = {})
{
  // 将颜色从SRGB空间转换到线性空间
  material.color.r = powf(material.color.r, 2.2f);
  material.color.g = powf(material.color.g, 2.2f);
  material.color.b = powf(material.color.b, 2.2f);
  material.color.a = powf(material.color.a, 2.2f);

  // 查找已存在的材质
  for(int materialIdx = 0; materialIdx < renderData->materials.count; materialIdx++)
  {
    if(renderData->materials[materialIdx] == material)
    {
      return materialIdx;
    }
  }

  // 添加新材质并返回索引
  return renderData->materials.add(material);
}

// 获取渲染层级
// @param layer: 基础层级
// @param subLayer: 子层级
// @return: 最终的渲染层级值
float get_layer(Layer layer, float subLayer = 0.0f)
{
  float floatLayer = (float)layer;
  float layerStep = 1.0f / (float)LAYER_COUNT;
  float result = layerStep * floatLayer + subLayer / 1000.0f;
  return result;
}

// 获取变换信息
// @param spriteID: 精灵ID
// @param pos: 位置
// @param size: 大小
// @param drawData: 绘制数据
// @return: 变换信息
Transform get_transform(SpriteID spriteID, Vec2 pos, Vec2 size = {}, DrawData drawData = {})
{
  Sprite sprite = get_sprite(spriteID);
  size = size? size: vec_2(sprite.size);

  Transform transform = {};
  transform.materialIdx = get_material_idx(drawData.material);
  transform.pos = pos - size / 2.0f;
  transform.size = size;
  transform.atlasOffset = sprite.atlasOffset;
  // 对于动画，这是基于animationIdx的精灵大小的倍数
  transform.atlasOffset.x += drawData.animationIdx * sprite.size.x;
  transform.spriteSize = sprite.size;
  transform.renderOptions = drawData.renderOptions;
  transform.layer = drawData.layer;

  return transform;
}

// #############################################################################
//                           Renderer Functions
// #############################################################################
// 绘制四边形
// @param transform: 变换信息
void draw_quad(Transform transform)
{
  renderData->transforms.add(transform);
}

// 绘制四边形
// @param pos: 位置
// @param size: 大小
// @param drawData: 绘制数据
void draw_quad(Vec2 pos, Vec2 size, DrawData drawData = {})
{
  Transform transform = get_transform(SPRITE_WHITE, pos, size, drawData);
  renderData->transforms.add(transform);
}

// 绘制精灵
// @param spriteID: 精灵ID
// @param pos: 位置
// @param drawData: 绘制数据
void draw_sprite(SpriteID spriteID, Vec2 pos, DrawData drawData = {})
{
  Transform transform = get_transform(spriteID, pos, {}, drawData);
  renderData->transforms.add(transform);
}

// 绘制精灵（整数坐标版本）
// @param spriteID: 精灵ID
// @param pos: 位置
// @param drawData: 绘制数据
void draw_sprite(SpriteID spriteID, IVec2 pos, DrawData drawData = {})
{
  draw_sprite(spriteID, vec_2(pos), drawData);
}

// #############################################################################
//                     Render Interface UI Rendering
// #############################################################################
// 绘制UI精灵
// @param spriteID: 精灵ID
// @param pos: 位置
// @param size: 大小
// @param drawData: 绘制数据
void draw_ui_sprite(SpriteID spriteID, Vec2 pos, Vec2 size = {}, DrawData drawData = {})
{
  Transform transform = get_transform(spriteID, pos, size, drawData);
  renderData->uiTransforms.add(transform);
}

// 绘制UI精灵（无大小参数版本）
// @param spriteID: 精灵ID
// @param pos: 位置
// @param drawData: 绘制数据
void draw_ui_sprite(SpriteID spriteID, Vec2 pos, DrawData drawData = {})
{
  Transform transform = get_transform(spriteID, pos, {}, drawData);
  renderData->uiTransforms.add(transform);
}

// 绘制UI精灵（整数坐标版本）
// @param spriteID: 精灵ID
// @param pos: 位置
// @param drawData: 绘制数据
void draw_ui_sprite(SpriteID spriteID, IVec2 pos, DrawData drawData = {})
{
  draw_ui_sprite(spriteID, vec_2(pos), drawData);
}

// #############################################################################
//                     Render Interface UI Font Rendering
// #############################################################################
// 绘制UI文本
// @param text: 文本内容
// @param pos: 位置
// @param textData: 文本数据
void draw_ui_text(char* text, Vec2 pos, TextData textData = {})
{
  SM_ASSERT(text, "No Text Supplied!");
  if(!text)
  {
    return;
  }

  Vec2 origin = pos;
  while(char c = *(text++))
  {
    if(c == '\n')
    {
      pos.y += renderData->fontHeight * textData.fontSize;
      pos.x = origin.x;
      continue;
    }

    Glyph glyph = renderData->glyphs[c];
    Transform transform = {};
    transform.materialIdx = get_material_idx(textData.material);
    transform.pos.x = pos.x + glyph.offset.x * textData.fontSize;
    transform.pos.y = pos.y - glyph.offset.y * textData.fontSize;
    transform.atlasOffset = glyph.textureCoords;
    transform.spriteSize = glyph.size;
    transform.size = vec_2(glyph.size) * textData.fontSize;
    transform.renderOptions = textData.renderOptions | RENDERING_OPTION_FONT;
    transform.layer = textData.layer;

    renderData->uiTransforms.add(transform);

    // 前进到下一个字形
    pos.x += glyph.advance.x * textData.fontSize;
  }
}

// 格式化绘制UI文本
// @param format: 格式化字符串
// @param pos: 位置
// @param args: 格式化参数
template <typename... Args>
void draw_format_ui_text(char* format, Vec2 pos, Args... args)
{
  char* text = format_text(format, args...);
  draw_ui_text(text, pos);
}