#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <vector>
#include <string>
#include "Globals.h"
#include "Background.h"

// Clasa MainMenu gestionează meniul principal al jocului.
// Aceasta permite jucătorului să înceapă jocul sau să iasă din aplicație.

class MainMenu {
public:
    MainMenu();   // Constructor – inițializează selecția implicită
    ~MainMenu();  // Destructor – eliberează memoria asociată cu texturi și sunete

    // Încarcă logo-ul, fundalul și sunetele necesare meniului
    bool loadAssets(SDL_Renderer* renderer);

    // Procesează inputul utilizatorului din meniu (navigare sus/jos, Enter pentru selecție)
    // Modifică flag-urile pentru pornirea jocului sau ieșire
    void handleInput(SDL_Event& event, bool& shouldStartGame, bool& shouldQuit);

    // Afișează meniul pe ecran: fundal, logo și opțiuni (cu selecția curentă evidențiată)
    void render(SDL_Renderer* renderer, TTF_Font* optionsFont);

private:
    std::vector<std::string> options;  // Lista de opțiuni (ex: Start Game, Exit)
    int selectedOption;                // Indexul opțiunii curente selectate de utilizator

    SDL_Texture* logoTexture = nullptr;       // Textura cu logo-ul jocului
    SDL_Texture* backgroundTexture = nullptr; // Fundalul meniului
    Mix_Chunk* confirmSound = nullptr;        // Sunet de confirmare la selecție
};
