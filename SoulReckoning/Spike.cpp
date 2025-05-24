#include "Spike.h"

Spike::Spike(int x, int y, SDL_Texture* tex) {
    rect = { x, y, 40, 40 };  // dimensiune spike
    texture = tex;
}

void Spike::render(SDL_Renderer* renderer, int cameraX) {
    SDL_Rect screenPos = { rect.x - cameraX, rect.y, rect.w, rect.h };
    SDL_RenderCopy(renderer, texture, nullptr, &screenPos);
}

SDL_Rect Spike::getCollider() const {
    return rect;
}
