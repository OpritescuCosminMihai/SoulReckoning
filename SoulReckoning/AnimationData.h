#pragma once
#include <SDL.h>

// Structura AnimationData reține toate informațiile necesare pentru o animație 2D.
// Este folosită atât de clasa Player cât și de Enemy pentru a reda corect animațiile în funcție de starea curentă.

struct AnimationData {
    SDL_Texture* texture = nullptr; // Textura sprite sheet-ului asociat acestei animații (încărcată cu SDL)

    int frameWidth = 0;             // Lățimea fiecărui cadru individual din sprite sheet
    int frameHeight = 0;            // Înălțimea fiecărui cadru

    int totalFrames = 0;            // Numărul total de cadre din această animație (de obicei coloane în sprite sheet)

    float animationSpeed = 0.1f;    // Cât durează fiecare cadru, exprimat în secunde (ex. 0.1 = 10 FPS)
};
