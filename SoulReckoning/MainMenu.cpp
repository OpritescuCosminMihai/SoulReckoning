#include "MainMenu.h"
#include <SDL_image.h>
#include <iostream>

MainMenu::MainMenu() {
    options = { "Start Game", "Exit" };
    selectedOption = 0;
}

MainMenu::~MainMenu() {
    if (logoTexture)
        SDL_DestroyTexture(logoTexture);
    if (backgroundTexture)
        SDL_DestroyTexture(backgroundTexture);
    if (confirmSound)
        Mix_FreeChunk(confirmSound);

}

bool MainMenu::loadAssets(SDL_Renderer* renderer) {
    logoTexture = IMG_LoadTexture(renderer, "D:\\PlatformerGame\\Menu\\soul_reckoning_logo.png");
    if (!logoTexture) {
        std::cerr << "Eroare la încărcarea logo-ului: " << IMG_GetError() << std::endl;
    }
    backgroundTexture = IMG_LoadTexture(renderer, "D:\\PlatformerGame\\Menu\\menu_bg.jpeg");
    if (!backgroundTexture) {
        std::cerr << "Eroare la încărcarea fundalului de meniu: " << IMG_GetError() << std::endl;
        return false;
    }
    confirmSound = Mix_LoadWAV("D:\\PlatformerGame\\Audio\\start.wav");
    if (!confirmSound) {
        std::cerr << "Eroare la încărcarea sunetului: " << Mix_GetError() << std::endl;
    }

}

void MainMenu::handleInput(SDL_Event& event, bool& shouldStartGame, bool& shouldQuit) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_UP:
            selectedOption = (selectedOption - 1 + options.size()) % options.size();
            break;
        case SDLK_DOWN:
            selectedOption = (selectedOption + 1) % options.size();
            break;
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            if (confirmSound) {
                Mix_Volume(-1, MIX_MAX_VOLUME); // setează volumul pe toate canalele la maxim
                Mix_VolumeChunk(confirmSound, MIX_MAX_VOLUME); // setează și volumul chunk-ului
                Mix_PlayChannel(-1, confirmSound, 0); // 🔊 sunet
            }
            if (selectedOption == 0) {
                shouldStartGame = true;
            }
            else if (selectedOption == 1) {
                shouldQuit = true;
            }
            break;
        }
    }
}


void MainMenu::render(SDL_Renderer* renderer, TTF_Font* optionsFont) {
    if (backgroundTexture) {
        SDL_Rect bgRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, &bgRect);
    }
    if (logoTexture) {
        SDL_Rect logoRect = { 30, 30, 300, 150 }; // sau alte dimensiuni potrivite
        SDL_RenderCopy(renderer, logoTexture, nullptr, &logoRect);
    }

    // Meniu
    int y = 500; // mai jos pe ecran
    int xOffset = 100; // mai la stânga

    for (size_t i = 0; i < options.size(); ++i) {
        SDL_Color color = (i == selectedOption) ? SDL_Color{ 255, 255, 0, 255 } : SDL_Color{ 255, 0, 0, 255 };

        SDL_Surface* surface = TTF_RenderText_Solid(optionsFont, options[i].c_str(), color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        int textW, textH;
        SDL_QueryTexture(texture, nullptr, nullptr, &textW, &textH);

        float scale = (i == selectedOption) ? 1.2f : 1.0f;

        int scaledW = static_cast<int>(textW * scale);
        int scaledH = static_cast<int>(textH * scale);
        int posX = xOffset;
        int posY = y - (scaledH - textH) / 2;

        SDL_Rect dst = { posX, posY, scaledW, scaledH };

        SDL_RenderCopy(renderer, texture, nullptr, &dst);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        y += 80;  // poți ajusta spacing dacă pare prea apropiat când e mărit
    }
}
