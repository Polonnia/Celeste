#define NOMINMAX
#include "Player.h"
#include <cmath>
#include <algorithm>  // Add this include for std::min
#include "Input.h"
#include "Level.h"

Player::Player(float x, float y)
    : position(x, y)
    , velocity(0, 0)
    , size(32, 32)
    , isGrounded(false)
    , jumpForce(400.0f)
    , moveSpeed(200.0f)
    , gravity(800.0f)
    , currentState(State::Normal)
    , dashTime(0)
    , dashCooldown(0)
    , wallSlideSpeed(100.0f)
    , climbSpeed(150.0f)
    , facingRight(true)
{
    // Initialize input state
    update_input();
    
    // Load sprites
    idleSprite.load(SpriteID::SPRITE_CELESTE);
    runSprite.load(SpriteID::SPRITE_CELESTE_RUN);
    jumpSprite.load(SpriteID::SPRITE_CELESTE_JUMP);
    dashSprite.load(SpriteID::SPRITE_CELESTE);
    wallSlideSprite.load(SpriteID::SPRITE_CELESTE);
    climbSprite.load(SpriteID::SPRITE_CELESTE);
}

Player::~Player() {
}

void Player::update(Level* level, float dt) {
    switch (currentState) {
        case State::Normal:
            handleNormalState(level, dt);
            break;
        case State::Dashing:
            handleDashState(level, dt);
            break;
        case State::WallSliding:
            handleWallSlideState(level, dt);
            break;
        case State::Climbing:
            handleClimbState(level, dt);
            break;
    }
}

void Player::handleNormalState(Level* level, float dt) {
    // Apply gravity
    velocity.y += gravity * dt;
    
    // Update position
    position += velocity * dt;
    
    // Handle collisions
    handleCollisions(level);
}

void Player::handleDashState(Level* level, float dt) {
    dashTime -= dt;
    if (dashTime <= 0) {
        dashTime = 0;
        dashCooldown = DASH_COOLDOWN;
        setState(State::Normal);
    }
    
    // Update position
    position += velocity * dt;
    
    // Handle collisions
    handleCollisions(level);
}

void Player::handleWallSlideState(Level* level, float dt) {
    velocity.y = std::min(velocity.y, wallSlideSpeed);
    
    // Update position
    position += velocity * dt;
    
    // Handle collisions
    handleCollisions(level);
}

void Player::handleClimbState(Level* level, float dt) {
    // Climbing logic
    velocity.y = -climbSpeed;
    
    // Update position
    position += velocity * dt;
    
    // Handle collisions
    handleCollisions(level);
}

void Player::setState(State newState) {
    if (currentState == newState) return;
    
    // Exit current state
    switch (currentState) {
        case State::Dashing:
            velocity.x = 0;
            velocity.y = 0;
            break;
        case State::WallSliding:
            velocity.y = 0;
            break;
        case State::Climbing:
            velocity.y = 0;
            break;
        default:
            break;
    }
    
    // Enter new state
    currentState = newState;
    switch (newState) {
        case State::Dashing:
            dashTime = DASH_TIME;
            // Set dash direction based on input
            if (key_is_down(KeyCodeID::KEY_A)) velocity.x = -DASH_SPEED;
            else if (key_is_down(KeyCodeID::KEY_D)) velocity.x = DASH_SPEED;
            else velocity.x = DASH_SPEED; // Default to right
            break;
        case State::WallSliding:
            wallSlideSpeed = WALL_SLIDE_SPEED;
            break;
        case State::Climbing:
            climbSpeed = CLIMB_SPEED;
            break;
        default:
            break;
    }
}

void Player::handleCollisions(Level* level) {
    // Horizontal collision
    if (level->checkWallCollision(position, size, velocity.x, true)) {
        velocity.x = 0;
    }
    
    // Vertical collision
    float depth;
    if (level->checkWallCollision(position, size, depth, false)) {
        position.y += depth;
        velocity.y = 0;
        isGrounded = depth < 0;
    } else {
        isGrounded = false;
    }
}

void Player::moveLeft() {
    velocity.x = -moveSpeed;
    facingRight = false;
}

void Player::moveRight() {
    velocity.x = moveSpeed;
    facingRight = true;
}

void Player::jump() {
    if (isGrounded) {
        velocity.y = -jumpForce;
        isGrounded = false;
    }
}

void Player::handleInput() {
    // Handle movement
    if (key_is_down(KeyCodeID::KEY_A)) {
        moveLeft();
    }
    else if (key_is_down(KeyCodeID::KEY_D)) {
        moveRight();
    }
    else {
        velocity.x = 0;
    }

    // Handle jump
    if (key_pressed_this_frame(KeyCodeID::KEY_SPACE)) {
        jump();
    }

    // Handle dash
    if (key_pressed_this_frame(KeyCodeID::KEY_SHIFT) && dashCooldown <= 0) {
        setState(State::Dashing);
    }

    // Update dash cooldown
    if (dashCooldown > 0) {
        dashCooldown -= 1.0f / 60.0f; // Assuming 60 FPS
    }
}

void Player::render() {
    // Update sprite direction
    if (velocity.x > 0) facingRight = true;
    else if (velocity.x < 0) facingRight = false;
    
    // Select and render appropriate sprite based on state
    switch (currentState) {
        case State::Normal:
            if (isGrounded) {
                if (std::abs(velocity.x) > 0.1f) {
                    runSprite.render(position.x, position.y, !facingRight);
                } else {
                    idleSprite.render(position.x, position.y, !facingRight);
                }
            } else {
                jumpSprite.render(position.x, position.y, !facingRight);
            }
            break;
            
        case State::Dashing:
            dashSprite.render(position.x, position.y, !facingRight);
            break;
            
        case State::WallSliding:
            wallSlideSprite.render(position.x, position.y, !facingRight);
            break;
            
        case State::Climbing:
            climbSprite.render(position.x, position.y, !facingRight);
            break;
    }
} 