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

    // Jucător
    Player player(100, 500);
    player.loadAnimations(renderer.sdlRenderer);

    // Platforme
    std::vector<Platform> platforms = {
        {200, 550, 200, 20},
        {450, 500, 200, 20},
        {700, 450, 200, 20},
        {950, 400, 200, 20},
        {300, 350, 200, 20}
    };

    // Inamici
    std::vector<Enemy> enemies;
    Enemy e1(800, 590, ENEMY_FRAME_WIDTH, ENEMY_FRAME_HEIGHT); e1.loadAnimations(renderer.sdlRenderer);
    Enemy e2(1200, 590, ENEMY_FRAME_WIDTH, ENEMY_FRAME_HEIGHT); e2.loadAnimations(renderer.sdlRenderer);
    enemies.push_back(e1);
    enemies.push_back(e2);

    int playerHealth = 100;
    int score = 0;

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
    int pauseSelected = 0; // 0 = Resume, 1 = Exit
    SDL_Event event;

    while (running) {
        Uint32 now = SDL_GetTicks();
        float deltaTime = (now - lastTime) / 1000.0f;
        lastTime = now;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;

            // ESC funcționează mereu
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    isPaused = !isPaused;
                }

                // Restul se execută doar dacă e în pauză
                if (isPaused) {
                    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN) {
                        pauseSelected = 1 - pauseSelected;
                    }

                    if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER) {
                        if (pauseSelected == 0) {
                            isPaused = false;
                        }
                        else {
                            running = false;
                        }
                    }
                }
            }
        }

        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        if (!isPaused) {
            player.handleInput(keystate, deltaTime);
            player.update(deltaTime, platforms);

            for (auto& enemy : enemies)
                enemy.update(deltaTime, player.x);

            // Atac
            if (player.currentState == PlayerState::ATTACK &&
                player.currentFrame == 1 &&
                !player.attackDamageApplied) {
                SDL_Rect hitbox = player.getAttackHitbox();
                for (auto& enemy : enemies) {
                    if (SDL_HasIntersection(&hitbox, &enemy.rect)) {
                        enemy.health -= 50;
                        std::cout << "Inamic lovit! HP: " << enemy.health << std::endl;
                        if (enemy.health <= 0) {
                            score += 100;
                            enemy.setState(EnemyState::DEAD);
                        }
                        else {
                            enemy.setState(EnemyState::HURT);
                        }
                    }
                }
                player.attackDamageApplied = true;
            }

            enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                [](const Enemy& e) { return e.health <= 0; }), enemies.end());

            int cameraX = static_cast<int>(player.x) - SCREEN_WIDTH / 2;

            // Randare
            renderer.clear();
            background.render(renderer.sdlRenderer, cameraX, 0);
            for (auto& p : platforms) p.render(renderer.sdlRenderer, cameraX);
            for (auto& e : enemies) e.render(renderer.sdlRenderer, cameraX);
            player.render(renderer.sdlRenderer, cameraX);
            renderHUD(renderer.sdlRenderer, font, playerHealth, score);
        } else {
            // Fundal blurat sau overlay semi-transparent
            SDL_SetRenderDrawBlendMode(renderer.sdlRenderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer.sdlRenderer, 0, 0, 0, 160); // semi-transparent negru
            SDL_Rect overlay = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
            SDL_RenderFillRect(renderer.sdlRenderer, &overlay);

            // Text: Resume / Exit
            const char* options[2] = { "Resume", "Exit to Menu" };
            int y = 250;
            for (int i = 0; i < 2; ++i) {
                SDL_Color color = (i == pauseSelected) ? SDL_Color{ 255, 255, 0, 255 } : SDL_Color{ 255, 0, 0, 255 };
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
    renderer.close();
    return 0;
}
