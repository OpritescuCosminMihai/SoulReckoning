// Platform.cpp
#include "Platform.h"

Platform::Platform(int x, int y, int w, int h) {
    rect = { x, y, w, h };
}

void Platform::render(SDL_Renderer* renderer, int cameraX) const {
    SDL_Rect renderRect = rect;
    renderRect.x -= cameraX;
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &renderRect);
}
