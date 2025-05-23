#pragma once
#include <SDL.h>

// Constante globale
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int GROUND_LEVEL = 600;

const float PLAYER_SPEED = 300.0f;
const float GRAVITY = 1000.0f;
const float JUMP_FORCE = 500.0f;

const int ENEMY_FRAME_WIDTH = 85;
const int ENEMY_FRAME_HEIGHT = 90;
const float ENEMY_SPEED = 100.0f;
const float ENEMY_DETECTION_RANGE = 300.0f;
const float ENEMY_ATTACK_RANGE = 50.0f;

// Enum-uri globale
enum class PlayerState {
    STAND, WALK, RUN, JUMP, CROUCH, ATTACK
};

enum class EnemyState {
    STAND, WALK, ATTACK, HURT, DEAD
};
