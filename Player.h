#pragma once
#include "math.h"
#include "Sprite.h"

// Forward declarations
class Level;

class Player {
public:
    enum class State {
        Normal,     // Normal state
        Dashing,    // Dashing state
        WallSliding,// Wall sliding state
        Climbing    // Climbing state
    };

private:
    Vec2 position;
    Vec2 velocity;
    Vec2 size;
    bool isGrounded;
    float jumpForce;
    float moveSpeed;
    float gravity;
    
    // Movement states
    State currentState;
    float dashTime;        // Dash duration
    float dashCooldown;    // Dash cooldown
    float wallSlideSpeed;  // Wall slide speed
    float climbSpeed;      // Climb speed
    
    // Sprites
    Sprite idleSprite;
    Sprite runSprite;
    Sprite jumpSprite;
    Sprite dashSprite;
    Sprite wallSlideSprite;
    Sprite climbSprite;
    bool facingRight;
    
    // Constants
    static constexpr float DASH_SPEED = 15.0f;
    static constexpr float DASH_TIME = 0.2f;
    static constexpr float DASH_COOLDOWN = 0.4f;
    static constexpr float WALL_SLIDE_SPEED = 2.0f;
    static constexpr float CLIMB_SPEED = 3.0f;

public:
    Player(float x, float y);
    ~Player();

    void update(Level* level, float dt);
    void render();
    void handleInput();

    // Movement
    void moveLeft();
    void moveRight();
    void jump();
    void dash();
    void applyGravity(float dt);

    // Collision detection
    void handleCollisions(Level* level);
    
    // Getters
    const Vec2& getPosition() const { return position; }
    const Vec2& getVelocity() const { return velocity; }
    const Vec2& getSize() const { return size; }
    bool getIsGrounded() const { return isGrounded; }
    bool isFacingRight() const { return facingRight; }
    
    // Setters
    void setPosition(const Vec2& pos) { position = pos; }
    void setVelocity(const Vec2& vel) { velocity = vel; }
    void setIsGrounded(bool grounded) { isGrounded = grounded; }

    // State management
    void setState(State newState);
    void handleNormalState(Level* level, float dt);
    void handleDashState(Level* level, float dt);
    void handleWallSlideState(Level* level, float dt);
    void handleClimbState(Level* level, float dt);

    // Getters
    float getX() const { return position.x; }
    float getY() const { return position.y; }
    int getWidth() const { return static_cast<int>(size.x); }
    int getHeight() const { return static_cast<int>(size.y); }
    State getState() const { return currentState; }
}; 