#pragma once
#include <SDL.h>

class Spike {
public:
    Spike(int x, int y, SDL_Texture* tex);

    void render(SDL_Renderer* renderer, int cameraX);
    SDL_Rect getCollider() const;

private:
    SDL_Rect rect;
    SDL_Texture* texture;
};
