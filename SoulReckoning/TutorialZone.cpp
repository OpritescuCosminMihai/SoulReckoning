#include "TutorialZone.h"
#include <iostream>

void TutorialZone::init(SDL_Renderer* renderer, TTF_Font* font) {
    this->font = font;

    // Inamic
    

    // Instrucțiuni
    SDL_Color color = { 255, 255, 255, 255 };
    std::string text = "<- / ->  - MOVE\nSpace   - JUMP\nA       - ATTACK\nD       - DODGE";
    SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), color, 400);
    instructionsTexture = SDL_CreateTextureFromSurface(renderer, surface);
    instructionsRect = { 80, 400, surface->w, surface->h };
    SDL_FreeSurface(surface);
}

void TutorialZone::render(SDL_Renderer* renderer) {
    for (auto& p : platforms) p.render(renderer, 0);
    for (auto& e : enemies) e.render(renderer, 0);
    if (instructionsTexture)
        SDL_RenderCopy(renderer, instructionsTexture, nullptr, &instructionsRect);
}