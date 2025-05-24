// Platform.h
#pragma once
#include <SDL.h>
#include "Globals.h"

class Platform {
public:
    SDL_Rect rect;
    SDL_Texture* texture = nullptr;

    Platform(int x, int y, int w, int h, SDL_Texture* tex);
    void setTexture(SDL_Texture* tex);
    void render(SDL_Renderer* renderer, int cameraX);
};

