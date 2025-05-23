// ResourceManager.cpp
#include "ResourceManager.h"

std::unordered_map<std::string, SDL_Texture*>& ResourceManager::getTextureMap() {
    static std::unordered_map<std::string, SDL_Texture*> textureMap;
    return textureMap;
}

SDL_Texture* ResourceManager::getTexture(SDL_Renderer* renderer, const std::string& path) {
    auto& textureMap = getTextureMap();
    if (textureMap.find(path) == textureMap.end()) {
        SDL_Texture* tex = IMG_LoadTexture(renderer, path.c_str());
        if (!tex) {
            std::cerr << "Nu s-a putut incarca " << path << ": " << IMG_GetError() << std::endl;
        }
        textureMap[path] = tex;
    }
    return textureMap[path];
}

void ResourceManager::clear() {
    auto& textureMap = getTextureMap();
    for (auto& pair : textureMap) {
        SDL_DestroyTexture(pair.second);
    }
    textureMap.clear();
}