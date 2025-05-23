// MainMenu.h
#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <vector>
#include <string>
#include "Globals.h"
#include "Background.h"

class MainMenu {
public:
    MainMenu();
    ~MainMenu();

    bool loadAssets(SDL_Renderer* renderer);
    void handleInput(SDL_Event& event, bool& shouldStartGame, bool& shouldQuit);
    void render(SDL_Renderer* renderer, TTF_Font* optionsFont);

private:
    std::vector<std::string> options;
    int selectedOption;

    SDL_Texture* logoTexture = nullptr;
    SDL_Texture* backgroundTexture = nullptr;
    Mix_Chunk* confirmSound = nullptr;
};

