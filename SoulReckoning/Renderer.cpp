// Renderer.cpp
#include "Renderer.h"

bool Renderer::init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Eroare la SDL_Init: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "Eroare la IMG_Init: " << IMG_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cerr << "Eroare la TTF_Init: " << TTF_GetError() << std::endl;
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Eroare la Mix_OpenAudio: " << Mix_GetError() << std::endl;
        return false;
    }
    else {
        std::cout << "[OK] Mix_OpenAudio a fost apelat cu succes.\n";
    }

    window = SDL_CreateWindow("Soul Reckoning", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Eroare la SDL_CreateWindow: " << SDL_GetError() << std::endl;
        return false;
    }

    sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!sdlRenderer) {
        std::cerr << "Eroare la SDL_CreateRenderer: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void Renderer::clear() {
    SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
    SDL_RenderClear(sdlRenderer);
    Mix_CloseAudio();
}

void Renderer::present() {
    SDL_RenderPresent(sdlRenderer);
}

void Renderer::close() {
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(window);
    ResourceManager::clear();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}