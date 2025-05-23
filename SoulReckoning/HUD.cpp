// HUD.cpp
#include "HUD.h"
#include <iostream>

void renderHUD(SDL_Renderer* renderer, TTF_Font* font, int playerHealth, int score) {
    std::string hudText = "Health: " + std::to_string(playerHealth) + "   Score: " + std::to_string(score);
    SDL_Color white = { 255, 255, 255, 255 };

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, hudText.c_str(), white);
    if (!textSurface) {
        std::cerr << "Eroare la creare text HUD: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        std::cerr << "Eroare la creare textura HUD: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
        return;
    }

    int textW, textH;
    SDL_QueryTexture(textTexture, nullptr, nullptr, &textW, &textH);
    SDL_Rect dstRect = { 10, 10, textW, textH };

    SDL_RenderCopy(renderer, textTexture, nullptr, &dstRect);

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}