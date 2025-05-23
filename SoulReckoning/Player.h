// Player.h
#pragma once
#include <SDL.h>
#include <unordered_map>
#include <vector>
#include <string>
#include "Globals.h"
#include "AnimationData.h"
#include "Platform.h"

class Player {
public:
    float x, y;
    int width, height;
    float velocityY;
    bool isJumping;
    bool facingRight;
    bool attackDamageApplied;

    std::unordered_map<PlayerState, AnimationData> animations;
    PlayerState currentState;
    int currentFrame;
    float animationTimer;
    SDL_Rect rect;

    Player(float startX, float startY);

    void loadAnimations(SDL_Renderer* renderer);
    void setState(PlayerState newState);
    void handleInput(const Uint8* keystate, float deltaTime);
    void update(float deltaTime, const std::vector<Platform>& platforms);
    void updateAnimation(float deltaTime);
    SDL_Rect getAttackHitbox();
    void render(SDL_Renderer* renderer, int cameraX);
};

