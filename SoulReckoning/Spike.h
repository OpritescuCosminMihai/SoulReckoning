#pragma once
#include <SDL.h>

// Clasa Spike reprezintă un obstacol static care provoacă moartea jucătorului la coliziune.
// Acesta este un element esențial pentru a adăuga dificultate și provocare în niveluri.

class Spike {
public:
    // Constructorul setează poziția și textura spike-ului.
    // Parametri:
    //   x, y - coordonatele spike-ului în lume
    //   tex - textura grafică folosită pentru desenare
    Spike(int x, int y, SDL_Texture* tex);

    // Desenează spike-ul pe ecran ținând cont de poziția camerei (cameraX)
    void render(SDL_Renderer* renderer, int cameraX);

    // Returnează dreptunghiul de coliziune al spike-ului, folosit pentru detecție cu jucătorul
    SDL_Rect getCollider() const;

private:
    SDL_Rect rect;        // Reprezintă poziția și dimensiunea spike-ului
    SDL_Texture* texture; // Textura asociată acestui spike
};
