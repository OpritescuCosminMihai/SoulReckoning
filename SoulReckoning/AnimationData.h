// AnimationData.h
#pragma once
#include <SDL.h>

struct AnimationData {
    SDL_Texture* texture = nullptr; // Textura înc?rcat?
    int frameWidth = 0;             // L??imea unui cadru
    int frameHeight = 0;            // În?l?imea unui cadru
    int totalFrames = 0;            // Num?rul total de cadre
    float animationSpeed = 0.1f;    // Timpul (în secunde) per cadru
};