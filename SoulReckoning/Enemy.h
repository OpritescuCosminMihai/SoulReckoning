// Enemy.h
#pragma once
#include <SDL.h>
#include <unordered_map>
#include <string>
#include "Globals.h"
#include "AnimationData.h"
#include "ResourceManager.h"

class Enemy {
public:
    float x, y;
    int width, height;
    int health;
    EnemyState currentState;
    std::unordered_map<EnemyState, AnimationData> animations;
    int currentFrame;
    float animationTimer;
    SDL_Rect rect;
    bool facingRight;

    float patrolMinX, patrolMaxX;
    bool patrolRight = true;

    Enemy(float startX, float startY, int w, int h);

    void loadAnimations(SDL_Renderer* renderer);
    void setState(EnemyState newState);
    void update(float deltaTime, float playerX);
    void render(SDL_Renderer* renderer, int cameraX);
};
