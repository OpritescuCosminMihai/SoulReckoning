// Platform.h
#pragma once
#include <SDL.h>
#include "Globals.h"

class Platform {
public:
    SDL_Rect rect;

    Platform(int x, int y, int w, int h);

    void render(SDL_Renderer* renderer, int cameraX) const;
};
