// main.cpp
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include "Globals.h"
#include "Renderer.h"
#include "Player.h"
#include "Enemy.h"
#include "Platform.h"
#include "ResourceManager.h"
#include "Background.h"
#include "HUD.h"
#include "MainMenu.h"
#include "Spike.h"

int main(int argc, char* argv[]) {
    Renderer renderer;
    if (!renderer.init()) return -1;

    // Font pentru HUD
    TTF_Font* font = TTF_OpenFont("D:\\PlatformerGame\\Fonts\\PressStart2P-Regular.ttf", 24);
    if (!font) {
        std::cerr << "Eroare font: " << TTF_GetError() << std::endl;
        return -1;
    }
    TTF_Font* titleFont = TTF_OpenFont("D:\\PlatformerGame\\Fonts\\JAPAN-Bold-Italic.ttf", 60); // pentru logo
    if (!titleFont) {
        std::cerr << "Eroare font: " << TTF_GetError() << std::endl;
        return -1;
    }
    TTF_Font* optionsFont = TTF_OpenFont("D:\\PlatformerGame\\Fonts\\JAPAN-Italic.ttf", 40); // pentru butoane
    if (!optionsFont) {
        std::cerr << "Eroare font: " << TTF_GetError() << std::endl;
        return -1;
    }

    // Fundal parallax
    MultiLayerBackground background;
    std::vector<std::string> bgPaths = {
        "D:\\PlatformerGame\\Backgrounds\\Layer_0011_0.png",
        "D:\\PlatformerGame\\Backgrounds\\Layer_0010_1.png",
        "D:\\PlatformerGame\\Backgrounds\\Layer_0009_2.png",
        "D:\\PlatformerGame\\Backgrounds\\Layer_0008_3.png",
        "D:\\PlatformerGame\\Backgrounds\\Layer_0007_Lights.png",
        "D:\\PlatformerGame\\Backgrounds\\Layer_0006_4.png",
        "D:\\PlatformerGame\\Backgrounds\\Layer_0005_5.png",
        "D:\\PlatformerGame\\Backgrounds\\Layer_0004_Lights.png",
        "D:\\PlatformerGame\\Backgrounds\\Layer_0003_6.png",
        "D:\\PlatformerGame\\Backgrounds\\Layer_0002_7.png",
        "D:\\PlatformerGame\\Backgrounds\\Layer_0001_8.png",
        "D:\\PlatformerGame\\Backgrounds\\Layer_0000_9.png"
    };
    std::vector<float> bgFactors = { 0.1f, 0.2f, 0.3f, 0.3f, 0.4f, 0.5f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f };
    if (!background.load(renderer.sdlRenderer, bgPaths, bgFactors)) {
        std::cerr << "Eroare fundal." << std::endl;
    }

    SDL_Texture* spikeTexture = nullptr;
    // Încarcă textura pentru spike (folosește BMP sau PNG cu SDL_image)
    SDL_Surface* spikeSurface = IMG_Load("D:\\PlatformerGame\\Enemies\\spike_up.png");
    if (!spikeSurface) {
        std::cerr << "Eroare incarcare spike: " << IMG_GetError() << std::endl;
        return -1;
    }
    spikeTexture = SDL_CreateTextureFromSurface(renderer.sdlRenderer, spikeSurface);
    SDL_FreeSurface(spikeSurface);
    // Spike-uri
    std::vector<Spike> spikes;
    for (int i = 0; i < 50; ++i) {
        spikes.emplace_back(500 + i * 40, 640, spikeTexture); // 40 = lățimea spike-ului
    }

    // Jucător
    Player player(100, 500);
    player.loadAnimations(renderer.sdlRenderer);

    SDL_Texture* platformLong = IMG_LoadTexture(renderer.sdlRenderer, "D:\\PlatformerGame\\Tiles\\platform_long.png");
    SDL_Texture* platformMedium1 = IMG_LoadTexture(renderer.sdlRenderer, "D:\\PlatformerGame\\Tiles\\platform_medium_1.png");
    SDL_Texture* platformMedium2 = IMG_LoadTexture(renderer.sdlRenderer, "D:\\PlatformerGame\\Tiles\\platform_medium_2.png");
    SDL_Texture* platformMedium3 = IMG_LoadTexture(renderer.sdlRenderer, "D:\\PlatformerGame\\Tiles\\platform_medium_3.png");
    SDL_Texture* platformSmall1 = IMG_LoadTexture(renderer.sdlRenderer, "D:\\PlatformerGame\\Tiles\\platform_small_1.png");
    SDL_Texture* platformSmall2 = IMG_LoadTexture(renderer.sdlRenderer, "D:\\PlatformerGame\\Tiles\\platform_small_2.png");
    SDL_Texture* platformSmall3 = IMG_LoadTexture(renderer.sdlRenderer, "D:\\PlatformerGame\\Tiles\\platform_small_3.png");

    // Platforme
    std::vector<Platform> platforms = {
    Platform(500, 560, 130, 48, platformSmall1),     // 1
    Platform(650, 470, 91, 50, platformSmall2),      // 2
    Platform(780, 430, 61, 35, platformSmall3),      // 3
    Platform(880, 470, 130, 48, platformSmall1),     // 4
    Platform(1050, 500, 225, 41, platformMedium1),   // 5
    Platform(1300, 460, 147, 41, platformMedium3),   // 6
    Platform(1500, 410, 91, 50, platformSmall2),     // 7
    Platform(1630, 460, 130, 48, platformSmall1),    // 8
    Platform(1780, 500, 177, 39, platformMedium2),   // 9
    Platform(2000, 460, 225, 41, platformMedium1),   // 10
    Platform(2250, 420, 130, 48, platformSmall1),    // 11
    Platform(2400, 390, 91, 50, platformSmall2),     // 12
    Platform(2550, 430, 61, 35, platformSmall3)      // 13
    };

    // Inamici
    std::vector<Enemy> enemies;

    int playerHealth = 100;
    int score = 0;

    int cameraShakeTimer = 0;     // câte frame-uri mai tremură camera
    int cameraShakeStrength = 30; // câți pixeli se deplasează camera la shake

    int hitFlashTimer = 0; // câte frame-uri mai afișăm flashul

    MainMenu menu;
    menu.loadAssets(renderer.sdlRenderer);
    bool inMenu = true;
    bool shouldStartGame = false;
    bool shouldQuit = false;

    Uint32 lastTime = SDL_GetTicks();

    while (inMenu) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                shouldQuit = true;
                inMenu = false;
            }
            menu.handleInput(event, shouldStartGame, shouldQuit);
        }

        if (shouldStartGame) {
            inMenu = false;
        }
        if (shouldQuit) {
            renderer.close();
            return 0;
        }

        menu.render(renderer.sdlRenderer, optionsFont);
        renderer.present();
    }

    // Main loop
    bool running = true;
    bool isPaused = false;

    bool playerDead = false;
    Uint32 deathTime = 0;
    int deathMenuSelection = 0;

    int pauseSelected = 0; // 0 = Resume, 1 = Exit
    SDL_Event event;

    while (running) {
        Uint32 now = SDL_GetTicks();
        float deltaTime = (now - lastTime) / 1000.0f;
        lastTime = now;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;

            if (!playerDead && event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    isPaused = !isPaused;
                }

                if (isPaused) {
                    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN) {
                        pauseSelected = 1 - pauseSelected;
                    }
                    if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER) {
                        if (pauseSelected == 0) isPaused = false;
                        else running = false;
                    }
                }
            }

            if (playerDead && SDL_GetTicks() - deathTime > 1000) {
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN)
                        deathMenuSelection = 1 - deathMenuSelection;

                    if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER) {
                        if (deathMenuSelection == 0) {
                            // RESTART
                            player.reset();
                            player.setState(PlayerState::STAND);
                            playerDead = false;
                            deathMenuSelection = 0;
                        }
                        else {
                            // EXIT TO MENU
                            running = false; // sau inMenu = true + continue;
                        }
                    }
                }
            }
        }

        // ------------------ ACTUALIZARE + RANDARE ------------------

        renderer.clear();
        int cameraX = static_cast<int>(player.x) - SCREEN_WIDTH / 2;
        background.render(renderer.sdlRenderer, cameraX, 0);
        for (auto& p : platforms) p.render(renderer.sdlRenderer, cameraX);
        for (auto& s : spikes) s.render(renderer.sdlRenderer, cameraX);
        for (auto& e : enemies) e.render(renderer.sdlRenderer, cameraX);

        if (!isPaused && !playerDead) {
            const Uint8* keystate = SDL_GetKeyboardState(NULL);
            player.handleInput(keystate, deltaTime);
            player.update(deltaTime, platforms);

            // enemy logic etc. (poți adăuga aici)
            for (auto& enemy : enemies)
                enemy.update(deltaTime, player.x);

            // coliziune spike
            SDL_Rect playerRect = player.getCollider();
            for (auto& spike : spikes) {
                SDL_Rect spikeRect = spike.getCollider();
                if (SDL_HasIntersection(&playerRect, &spikeRect)) {
                    player.setState(PlayerState::DEAD);
                    deathTime = SDL_GetTicks();
                    playerDead = true;
                    break;
                }
            }

            player.render(renderer.sdlRenderer, cameraX);
            renderHUD(renderer.sdlRenderer, font, playerHealth, score);
        }

        else if (playerDead) {
            player.update(deltaTime, platforms); // lasă animatia să ruleze
            player.render(renderer.sdlRenderer, cameraX);

            if (SDL_GetTicks() - deathTime > 1000) {
                SDL_SetRenderDrawBlendMode(renderer.sdlRenderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer.sdlRenderer, 0, 0, 0, 180);
                SDL_Rect overlay = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
                SDL_RenderFillRect(renderer.sdlRenderer, &overlay);

                SDL_Surface* title = TTF_RenderText_Solid(font, "YOU DIED", { 255, 0, 0 });
                SDL_Texture* titleTex = SDL_CreateTextureFromSurface(renderer.sdlRenderer, title);
                int textW, textH;
                SDL_QueryTexture(titleTex, nullptr, nullptr, &textW, &textH);
                SDL_Rect dstTitle = { SCREEN_WIDTH / 2 - textW / 2, 100, textW, textH };
                SDL_RenderCopy(renderer.sdlRenderer, titleTex, nullptr, &dstTitle);
                SDL_FreeSurface(title);
                SDL_DestroyTexture(titleTex);

                const char* options[] = { "RESTART", "EXIT TO MENU" };
                for (int i = 0; i < 2; ++i) {
                    SDL_Color color = (i == deathMenuSelection) ? SDL_Color{ 255, 255, 0 } : SDL_Color{ 200, 200, 200 };
                    SDL_Surface* optSurf = TTF_RenderText_Solid(font, options[i], color);
                    SDL_Texture* optTex = SDL_CreateTextureFromSurface(renderer.sdlRenderer, optSurf);
                    SDL_QueryTexture(optTex, nullptr, nullptr, &textW, &textH);
                    SDL_Rect dstOpt = { SCREEN_WIDTH / 2 - textW / 2, 200 + i * 60, textW, textH };
                    SDL_RenderCopy(renderer.sdlRenderer, optTex, nullptr, &dstOpt);
                    SDL_FreeSurface(optSurf);
                    SDL_DestroyTexture(optTex);
                }
            }
        }

        else if (isPaused) {
            SDL_SetRenderDrawBlendMode(renderer.sdlRenderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer.sdlRenderer, 0, 0, 0, 160);
            SDL_Rect overlay = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
            SDL_RenderFillRect(renderer.sdlRenderer, &overlay);

            const char* options[2] = { "Resume", "Exit to Menu" };
            int y = 250;
            for (int i = 0; i < 2; ++i) {
                SDL_Color color = (i == pauseSelected) ? SDL_Color{ 255, 255, 0 } : SDL_Color{ 255, 0, 0 };
                SDL_Surface* surface = TTF_RenderText_Solid(optionsFont, options[i], color);
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer.sdlRenderer, surface);
                int textW, textH;
                SDL_QueryTexture(texture, nullptr, nullptr, &textW, &textH);
                SDL_Rect dst = { SCREEN_WIDTH / 2 - textW / 2, y, textW, textH };
                SDL_RenderCopy(renderer.sdlRenderer, texture, nullptr, &dst);
                SDL_FreeSurface(surface);
                SDL_DestroyTexture(texture);
                y += 80;
            }
        }

        renderer.present();
    }

    background.free();
    TTF_CloseFont(font);
    TTF_CloseFont(titleFont);
    TTF_CloseFont(optionsFont);
    SDL_DestroyTexture(spikeTexture);
    SDL_DestroyTexture(platformLong);
    SDL_DestroyTexture(platformMedium1);
    SDL_DestroyTexture(platformMedium2);
    SDL_DestroyTexture(platformMedium3);
    SDL_DestroyTexture(platformSmall1);
    SDL_DestroyTexture(platformSmall2);
    SDL_DestroyTexture(platformSmall3);
    renderer.close();
    return 0;
}
