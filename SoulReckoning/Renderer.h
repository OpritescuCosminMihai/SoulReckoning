#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include "ResourceManager.h"

// Clasa Renderer gestionează inițializarea, curățarea și afișarea scenei pe ecran.
// Este componenta centrală pentru randarea tuturor elementelor vizuale din joc.

class Renderer {
public:
    SDL_Window* window = nullptr;        // Fereastra principală a jocului
    SDL_Renderer* sdlRenderer = nullptr; // Rendererul SDL asociat ferestrei, folosit pentru toate operațiile de desenare

    // Inițializează SDL, creează fereastra și rendererul, și inițializează subsistemele (imagini, fonturi, sunet)
    bool init();

    // Curăță ecranul pentru un nou frame de randare
    void clear();

    // Afișează toate obiectele desenate în acest frame (swap buffers)
    void present();

    // Eliberează resursele create (fereastră, renderer, subsisteme)
    void close();
};
