#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include "Player.h"
#include "Enemy.h"
#include "Platform.h"

class TutorialZone {
public:
    void init(SDL_Renderer* renderer, TTF_Font* font);
    void render(SDL_Renderer* renderer);

    std::vector<Enemy> enemies;
    std::vector<Platform> platforms;

private:
    TTF_Font* font = nullptr;
    SDL_Texture* instructionsTexture = nullptr;
    SDL_Rect instructionsRect{};
};
