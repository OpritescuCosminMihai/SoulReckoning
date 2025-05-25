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
    // Încarcă textura pentru spike
    SDL_Surface* spikeSurface = IMG_Load("D:\\PlatformerGame\\Enemies\\spike_up.png");
    if (!spikeSurface) {
        std::cerr << "Eroare incarcare spike: " << IMG_GetError() << std::endl;
        return -1;
    }
    spikeTexture = SDL_CreateTextureFromSurface(renderer.sdlRenderer, spikeSurface);
    SDL_FreeSurface(spikeSurface);
    // Spike-uri
    std::vector<Spike> spikes;
    for (int i = 0; i < 130; ++i) {
        spikes.emplace_back(400 + i * 40, 640, spikeTexture); // 40 = lățimea spike-ului
    }

    // Jucător
    Player player(100, 550);
    player.loadAnimations(renderer.sdlRenderer);

    // Platforme
    SDL_Texture* platformLong = IMG_LoadTexture(renderer.sdlRenderer, "D:\\PlatformerGame\\Tiles\\platform_long.png");
    SDL_Texture* platformMedium1 = IMG_LoadTexture(renderer.sdlRenderer, "D:\\PlatformerGame\\Tiles\\platform_medium_1.png");
    SDL_Texture* platformMedium2 = IMG_LoadTexture(renderer.sdlRenderer, "D:\\PlatformerGame\\Tiles\\platform_medium_2.png");
    SDL_Texture* platformMedium3 = IMG_LoadTexture(renderer.sdlRenderer, "D:\\PlatformerGame\\Tiles\\platform_medium_3.png");
    SDL_Texture* platformSmall1 = IMG_LoadTexture(renderer.sdlRenderer, "D:\\PlatformerGame\\Tiles\\platform_small_1.png");
    SDL_Texture* platformSmall2 = IMG_LoadTexture(renderer.sdlRenderer, "D:\\PlatformerGame\\Tiles\\platform_small_2.png");
    SDL_Texture* platformSmall3 = IMG_LoadTexture(renderer.sdlRenderer, "D:\\PlatformerGame\\Tiles\\platform_small_3.png");

    std::vector<Platform> platforms = {
    Platform(500, 560, 130, 48, platformSmall1),     // 1
    Platform(650, 450, 91, 50, platformSmall2),      // 2
    Platform(1080, 470, 130, 48, platformSmall1),     // 3
    Platform(1600, 430, 61, 35, platformSmall3),      // 4
    Platform(2000, 500, 225, 41, platformMedium1),   // 5
    Platform(2100, 360, 147, 41, platformMedium3),   // 6
    Platform(2700, 410, 91, 50, platformSmall2),     // 7
    Platform(3100, 460, 130, 48, platformSmall1),    // 8
    Platform(3350, 320, 177, 39, platformMedium2),   // 9
    Platform(3900, 460, 225, 41, platformMedium1),   // 10
    Platform(4200, 340, 130, 48, platformSmall1),    // 11
    Platform(4800, 390, 91, 50, platformSmall2),     // 12
    Platform(5200, 430, 61, 35, platformSmall3)      // 13
    };

    // Inamici
    std::vector<Enemy> enemies;

    enemies.emplace_back(6100, 585, ENEMY_FRAME_WIDTH, ENEMY_FRAME_HEIGHT); // aproape de platforma 5
    enemies.emplace_back(6900, 585, ENEMY_FRAME_WIDTH, ENEMY_FRAME_HEIGHT); // înainte de platforma 9

    // Pentru fiecare inamic, încarcă animațiile
    for (auto& enemy : enemies) {
        enemy.loadAnimations(renderer.sdlRenderer);
    }

    int playerHealth = 100;
    int score = 0;

    const int DAMAGE_COOLDOWN = 3000; // ms între lovituri
    Uint32 lastDamageTime = 0;

    int cameraShakeTimer = 0;     // câte frame-uri mai tremură camera
    int hitFlashTimer = 0; // câte frame-uri mai afișăm flashul

    int cameraShakeStrength = 20; // câți pixeli se deplasează camera la shake

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
                            running = false;
                        }
                    }
                }
            }
        }

        // ------------------ ACTUALIZARE + RANDARE ------------------

        renderer.clear();
        int cameraX = static_cast<int>(player.x) - SCREEN_WIDTH / 2;

        if (cameraShakeTimer > 0) {
            int offset = (rand() % (2 * cameraShakeStrength + 1)) - cameraShakeStrength;
            cameraX += offset;
            cameraShakeTimer--;
        }
        background.render(renderer.sdlRenderer, cameraX, 0);
        for (auto& p : platforms) p.render(renderer.sdlRenderer, cameraX);
        for (auto& s : spikes) s.render(renderer.sdlRenderer, cameraX);
        for (auto& e : enemies) e.render(renderer.sdlRenderer, cameraX);

        if (!isPaused && !playerDead) {
            const Uint8* keystate = SDL_GetKeyboardState(NULL);
            player.handleInput(keystate, deltaTime);
            player.update(deltaTime, platforms);

            // ATACAREA INAMICILOR
            if (player.currentState == PlayerState::ATTACK &&
                player.currentFrame == 1 &&  // Frame-ul unde se aplică damage
                !player.attackDamageApplied) {

                SDL_Rect hitbox = player.getAttackHitbox();

                for (auto& enemy : enemies) {
                    if (SDL_HasIntersection(&hitbox, &enemy.rect)) {
                        enemy.health -= 20;
                        hitFlashTimer = 50;      
                        cameraShakeTimer = 200; 

                        // Direcția jucătorului determină direcția knockback-ului

                        enemy.knockbackSpeed = player.facingRight ? 100.0f : -100.0f;
                        enemy.knockbackTimer = 1.2f; // durează 1.2 secunde

                        if (enemy.health <= 0) {
                            enemy.setState(EnemyState::DEAD);
                        }
                        else {
                            enemy.setState(EnemyState::HURT);
                        }

                        player.attackDamageApplied = true; // doar o dată per atac
                    }
                }
            }

            if (hitFlashTimer > 0) {
                SDL_SetRenderDrawBlendMode(renderer.sdlRenderer, SDL_BLENDMODE_BLEND);

                int alpha = static_cast<int>((hitFlashTimer / 5.0f) * 100.0f);
                SDL_SetRenderDrawColor(renderer.sdlRenderer, 255, 0, 0, alpha);

                SDL_Rect flashRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
                SDL_RenderFillRect(renderer.sdlRenderer, &flashRect);

                hitFlashTimer--;
            }

            // enemy logic
            for (auto& enemy : enemies)
                enemy.update(deltaTime, player.x);

            // Detectare coliziune player <-> inamici
            if (!playerDead && SDL_GetTicks() - lastDamageTime > DAMAGE_COOLDOWN) {
                SDL_Rect playerRect = player.getCollider();
                for (auto& enemy : enemies) {
                    if (enemy.currentState == EnemyState::ATTACK && enemy.currentFrame == 3 && !enemy.hasDealtDamage && SDL_HasIntersection(&playerRect, &enemy.rect)) {
                        if (player.currentState == PlayerState::BLOCK) {
                            // Efect de block (sunet, scânteie etc.)
                        }
                        else {
                            playerHealth -= 20; // sau orice damage vrei
                            hitFlashTimer = 50; // efect de damage
                            std::cout << "Player lovit! HP: " << playerHealth << std::endl;

                            if (playerHealth <= 0) {
                                player.setState(PlayerState::DEAD);
                                deathTime = SDL_GetTicks();
                                playerDead = true;
                                break;
                            }
                        }
                        enemy.hasDealtDamage = true;
                        player.knockbackSpeedX = enemy.facingRight ? 200.0f : -200.0f;
                        player.knockbackSpeedY = -150.0f;
                        player.knockbackDuration = 0.4f;
                        player.beingKnockedBack = true;
                    }
                }
            }

            // Eliminăm inamicii morți
            enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                [](const Enemy& e) { return e.health <= 0; }), enemies.end());

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
