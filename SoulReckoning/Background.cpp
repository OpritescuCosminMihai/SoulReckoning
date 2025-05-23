// Background.cpp
#include "Background.h"

bool MultiLayerBackground::load(SDL_Renderer* renderer, const std::vector<std::string>& paths, const std::vector<float>& factors) {
    if (paths.size() != factors.size()) {
        std::cerr << "Numarul de cai si factori de parallax trebuie sa fie egal." << std::endl;
        return false;
    }

    for (size_t i = 0; i < paths.size(); i++) {
        SDL_Texture* tex = ResourceManager::getTexture(renderer, paths[i]);
        if (!tex) return false;
        layers.push_back(tex);
        parallaxFactors.push_back(factors[i]);
    }

    return true;
}

void MultiLayerBackground::render(SDL_Renderer* renderer, int cameraX, int cameraY) {
    for (size_t i = 0; i < layers.size(); i++) {
        float factor = parallaxFactors[i];
        int offsetX = static_cast<int>(cameraX * factor);
        int texW, texH;
        SDL_QueryTexture(layers[i], nullptr, nullptr, &texW, &texH);
        int startX = -(offsetX % texW);
        if (startX > 0)
            startX -= texW;

        for (int x = startX; x < 1280; x += texW) {
            SDL_Rect destRect = { x, 0, texW, 720 };
            SDL_RenderCopy(renderer, layers[i], nullptr, &destRect);
        }
    }
}

void MultiLayerBackground::free() {
    layers.clear();
    parallaxFactors.clear();
}
