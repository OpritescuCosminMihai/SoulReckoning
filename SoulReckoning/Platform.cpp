// Platform.cpp
#include "Platform.h"

Platform::Platform(int x, int y, int w, int h, SDL_Texture* tex) {
    rect = { x, y, w, h };
    texture = tex;
}

void Platform::setTexture(SDL_Texture* tex) {
    texture = tex;
}

void Platform::render(SDL_Renderer* renderer, int cameraX) {
    SDL_Rect screenRect = { rect.x - cameraX, rect.y, rect.w, rect.h };
    if (texture)
        SDL_RenderCopy(renderer, texture, nullptr, &screenRect);
    else {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &screenRect);
    }
}
