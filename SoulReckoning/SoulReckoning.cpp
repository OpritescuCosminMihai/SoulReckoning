// main.cpp
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include "Globals.h"
#include "Renderer.h"
#include "Player.h"
#include "Enemy.h"
#include "Platform.h"
#include "ResourceManager.h"
#include "Background.h"
#include "HUD.h"

int main(int argc, char* argv[]) {
    Renderer renderer;
    if (!renderer.init()) return -1;

    // Font pentru HUD
    TTF_Font* font = TTF_OpenFont("D:\\PlatformerGame\\Fonts\\PressStart2P-Regular.ttf", 24);
    if (!font) {
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

    // Main loop
    bool running = true;
    SDL_Event event;
    Uint32 lastTime = SDL_GetTicks();

    while (running) {
        Uint32 now = SDL_GetTicks();
        float deltaTime = (now - lastTime) / 1000.0f;
        lastTime = now;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        const Uint8* keystate = SDL_GetKeyboardState(NULL);
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

        for (auto& p : platforms)
            p.render(renderer.sdlRenderer, cameraX);

        for (auto& e : enemies)
            e.render(renderer.sdlRenderer, cameraX);

        player.render(renderer.sdlRenderer, cameraX);

        renderHUD(renderer.sdlRenderer, font, playerHealth, score);
        renderer.present();
    }

    background.free();
    TTF_CloseFont(font);
    renderer.close();
    return 0;
}
