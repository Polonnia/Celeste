#pragma once
#include "math.h"

class Camera {
public:
    Camera();
    ~Camera();

    // Position
    void setPosition(const Vec2& pos);
    const Vec2& getPosition() const;

    // Transform
    void applyTransform();
    void resetTransform();

    // Rotation
    void setRotation(float angle);
    float getRotation() const;

    // Zoom
    void setZoom(float scale);
    float getZoom() const;

    // Update
    void update(float dt);

    void setTarget(const Vec2& newTarget);
    const Vec2& getTarget() const;
    float getScale() const;
    void setScale(float newScale);

public: // Making these public temporarily to fix compilation
    Vec2 position;
    Vec2 target;
    float width;
    float height;
    float scale;
    float rotation;
    bool isDirty;
};

// �������λ��
void update_camera(Camera* camera, const Vec2& target);

// ��ȡ����任����
void get_camera_transform(const Camera* camera, float* transform); 