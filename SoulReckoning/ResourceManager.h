// ResourceManager.h
#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <unordered_map>
#include <string>
#include <iostream>

class ResourceManager {
public:
    static SDL_Texture* getTexture(SDL_Renderer* renderer, const std::string& path);
    static void clear();

private:
    static std::unordered_map<std::string, SDL_Texture*>& getTextureMap();
};
