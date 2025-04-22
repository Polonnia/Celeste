#include "Camera.h"
#include <cmath>

// 声明数学函数
#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif

Camera::Camera()
    : position(0, 0)
    , target(0, 0)
    , width(800.0f)    // 默认窗口大小
    , height(600.0f)   // 默认窗口大小
    , scale(1.0f)
    , rotation(0.0f)
    , isDirty(true)
{
}

Camera::~Camera()
{
}

void Camera::setPosition(const Vec2& pos)
{
    position = pos;
    isDirty = true;
}

const Vec2& Camera::getPosition() const
{
    return position;
}

void Camera::setTarget(const Vec2& newTarget)
{
    target = newTarget;
    isDirty = true;
}

const Vec2& Camera::getTarget() const
{
    return target;
}

float Camera::getScale() const
{
    return scale;
}

void Camera::setScale(float newScale)
{
    scale = newScale;
    isDirty = true;
}

void Camera::setRotation(float angle)
{
    rotation = angle;
    isDirty = true;
}

float Camera::getRotation() const
{
    return rotation;
}

void Camera::setZoom(float newZoom)
{
    scale = newZoom;  // 使用scale作为zoom
    isDirty = true;
}

float Camera::getZoom() const
{
    return scale;  // 返回scale作为zoom
}

void Camera::applyTransform()
{
    if (!isDirty) return;

    // 计算相机的最终位置（考虑目标位置）
    Vec2 finalPos = position;
    
    // 如果有目标，进行插值
    if (target.x != position.x || target.y != position.y)
    {
        const float smoothSpeed = 5.0f;
        float dt = 1.0f / 60.0f; // 假设60FPS
        finalPos.x += (target.x - position.x) * smoothSpeed * dt;
        finalPos.y += (target.y - position.y) * smoothSpeed * dt;
        position = finalPos;
    }

    // 更新变换矩阵
    float transform[6];
    get_camera_transform(this, transform);
    
    isDirty = false;
}

void Camera::resetTransform()
{
    position = Vec2(0, 0);
    target = Vec2(0, 0);
    scale = 1.0f;
    rotation = 0.0f;
    isDirty = true;
}

void Camera::update(float dt)
{
    if (target.x != position.x || target.y != position.y)
    {
        const float smoothSpeed = 5.0f;
        position.x += (target.x - position.x) * smoothSpeed * dt;
        position.y += (target.y - position.y) * smoothSpeed * dt;
        isDirty = true;
    }
}

// 全局函数实现
void update_camera(Camera* camera, const Vec2& target)
{
    camera->setTarget(target);
    camera->update(1.0f / 60.0f);
}

void get_camera_transform(const Camera* camera, float* transform)
{
    float c = static_cast<float>(cos(camera->getRotation()));
    float s = static_cast<float>(sin(camera->getRotation()));
    float zoom = camera->getScale();
    
    // 构建2D变换矩阵
    transform[0] = c * zoom;  // m11
    transform[1] = -s * zoom; // m12
    transform[2] = -camera->position.x * transform[0] - camera->position.y * transform[1];  // tx
    transform[3] = s * zoom;  // m21
    transform[4] = c * zoom;  // m22
    transform[5] = -camera->position.x * transform[3] - camera->position.y * transform[4];  // ty
} 