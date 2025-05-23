// Renderer.h
#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include "ResourceManager.h"

class Renderer {
public:
    SDL_Window* window = nullptr;
    SDL_Renderer* sdlRenderer = nullptr;

    bool init();
    void clear();
    void present();
    void close();
};
