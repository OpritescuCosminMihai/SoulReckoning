#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <unordered_map>
#include <string>
#include <iostream>

// Clasa ResourceManager este responsabilă pentru gestionarea și reutilizarea texturilor.
// Aceasta previne încărcarea multiplă a acelorași fișiere imagine, reducând consumul de memorie
// și îmbunătățind performanța jocului.

class ResourceManager {
public:
    // Returnează textura asociată cu un anumit path.
    // Dacă textura a mai fost încărcată anterior, este returnată din cache.
    // Dacă nu, este încărcată de pe disc și salvată pentru utilizări viitoare.
    static SDL_Texture* getTexture(SDL_Renderer* renderer, const std::string& path);

    // Eliberează toate texturile încărcate și goliște cache-ul.
    // Se apelează la închiderea jocului pentru a preveni memory leaks.
    static void clear();

private:
    // Returnează referința către mapa internă de texturi.
    // Această mapă asociază path-urile fișierelor cu texturile încărcate.
    static std::unordered_map<std::string, SDL_Texture*>& getTextureMap();
};
