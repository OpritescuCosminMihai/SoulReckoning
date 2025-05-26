#pragma once
#include <SDL.h>
#include <vector>
#include <string>
#include <iostream>
#include "ResourceManager.h"

// Clasa MultiLayerBackground gestionează randarea fundalului în straturi (parallax scrolling).
// Fiecare strat se mișcă cu o viteză diferită pentru a crea iluzia de profunzime.

class MultiLayerBackground {
public:
    std::vector<SDL_Texture*> layers;         // Vector de texturi ce reprezintă straturile fundalului
    std::vector<float> parallaxFactors;       // Factori de parallax (viteză de deplasare relativă pentru fiecare strat)

    // Încarcă texturile fundalului și le asociază cu factori de parallax corespunzători
    bool load(SDL_Renderer* renderer, const std::vector<std::string>& paths, const std::vector<float>& factors);

    // Afișează straturile fundalului în funcție de poziția camerei (X și Y)
    void render(SDL_Renderer* renderer, int cameraX, int cameraY);

    // Eliberează memoria ocupată de texturile fundalului
    void free();
};
