#pragma once
#include <SDL.h>
#include "Globals.h"

// Clasa Platform reprezintă o platformă solidă din joc pe care jucătorul și inamicii pot sta sau interacționa.
// Fiecare platformă are o poziție, dimensiuni și o textură grafică.

class Platform {
public:
    SDL_Rect rect;                 // Reprezintă poziția și dimensiunea platformei pe ecran
    SDL_Texture* texture = nullptr; // Textura asociată platformei (imaginea vizuală)

    // Constructorul setează poziția, dimensiunea și textura inițială a platformei
    Platform(int x, int y, int w, int h, SDL_Texture* tex);

    // Permite schimbarea texturii platformei în timpul rulării
    void setTexture(SDL_Texture* tex);

    // Desenează platforma pe ecran, ținând cont de poziția camerei (pentru efectul de scroll)
    void render(SDL_Renderer* renderer, int cameraX);
};
