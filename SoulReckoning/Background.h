// Background.h
#pragma once
#include <SDL.h>
#include <vector>
#include <string>
#include <iostream>
#include "ResourceManager.h"

class MultiLayerBackground {
public:
    std::vector<SDL_Texture*> layers;
    std::vector<float> parallaxFactors;

    bool load(SDL_Renderer* renderer, const std::vector<std::string>& paths, const std::vector<float>& factors);
    void render(SDL_Renderer* renderer, int cameraX, int cameraY);
    void free();
};
