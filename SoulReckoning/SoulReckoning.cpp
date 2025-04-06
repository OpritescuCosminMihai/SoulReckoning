#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
//SOUL_RECKONING_GAME
using namespace std;

// Dimensiuni ecran
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

// Parametri pentru mișcare
const float PLAYER_SPEED = 300.0f;  // pixeli pe secundă
const float GRAVITY = 1000.0f;
const float JUMP_FORCE = 500.0f;
const int GROUND_LEVEL = 600;

// Dimensiuni inamic (cadru pentru animații)
const int ENEMY_FRAME_WIDTH = 85;
const int ENEMY_FRAME_HEIGHT = 90;

// Structură ce reține date despre o animație
struct AnimationData {
    SDL_Texture* texture = nullptr; // Textura încărcată
    int frameWidth = 0;             // Lățimea unui cadru
    int frameHeight = 0;            // Înălțimea unui cadru
    int totalFrames = 0;            // Numărul total de cadre
    float animationSpeed = 0.1f;    // Timpul (în secunde) per cadru
};

// Stările posibile ale jucătorului
enum class PlayerState {
    STAND,
    WALK,
    RUN,
    JUMP,
    CROUCH,
    ATTACK
};

// Stările posibile ale inamicilor
enum class EnemyState {
    STAND,
    WALK,
    ATTACK,
    HURT,
    DEAD
};

//--------------------------------------------------
// ResourceManager: gestionează încărcarea și reutilizarea texturilor
//--------------------------------------------------
class ResourceManager {
public:
    static unordered_map<string, SDL_Texture*>& getTextureMap() {
        static unordered_map<string, SDL_Texture*> textureMap;
        return textureMap;
    }

    static SDL_Texture* getTexture(SDL_Renderer* renderer, const string& path) {
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

    static void clear() {
        auto& textureMap = getTextureMap();
        for (auto& pair : textureMap) {
            SDL_DestroyTexture(pair.second);
        }
        textureMap.clear();
    }
};

//--------------------------------------------------
// Clasa pentru gestionarea ferestrei și renderer-ului
//--------------------------------------------------
class Renderer {
public:
    SDL_Window* window = nullptr;
    SDL_Renderer* sdlRenderer = nullptr;

    bool init() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "Eroare la SDL_Init: " << SDL_GetError() << std::endl;
            return false;
        }
        if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
            std::cerr << "Eroare la IMG_Init: " << IMG_GetError() << std::endl;
            return false;
        }
        // Inițializare SDL_ttf
        if (TTF_Init() == -1) {
            std::cerr << "Eroare la TTF_Init: " << TTF_GetError() << std::endl;
            return false;
        }
        window = SDL_CreateWindow("Soul Reckoning", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (!window) {
            std::cerr << "Eroare la SDL_CreateWindow: " << SDL_GetError() << std::endl;
            return false;
        }
        sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!sdlRenderer) {
            std::cerr << "Eroare la SDL_CreateRenderer: " << SDL_GetError() << std::endl;
            return false;
        }
        return true;
    }

    void clear() {
        SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
        SDL_RenderClear(sdlRenderer);
    }

    void present() {
        SDL_RenderPresent(sdlRenderer);
    }

    void close() {
        SDL_DestroyRenderer(sdlRenderer);
        SDL_DestroyWindow(window);
        ResourceManager::clear();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }
};

//--------------------------------------------------
// HUD: afișează informații pe ecran (de exemplu, Health și Score)
//--------------------------------------------------
void renderHUD(SDL_Renderer* renderer, TTF_Font* font, int playerHealth, int score) {
    string hudText = "Health: " + to_string(playerHealth) + "   Score: " + to_string(score);
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, hudText.c_str(), white);
    if (!textSurface) {
        std::cerr << "Eroare la crearea suprafeței text: " << TTF_GetError() << std::endl;
        return;
    }
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        std::cerr << "Eroare la crearea texturii text: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
        return;
    }
    int textW, textH;
    SDL_QueryTexture(textTexture, nullptr, nullptr, &textW, &textH);
    SDL_Rect dstRect = { 10, 10, textW, textH };
    SDL_RenderCopy(renderer, textTexture, nullptr, &dstRect);
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}

//--------------------------------------------------
// Clasa MultiLayerBackground – gestionează fundalul cu layere și efect de parallax
//--------------------------------------------------
class MultiLayerBackground {
public:
    vector<SDL_Texture*> layers;
    vector<float> parallaxFactors;

    bool load(SDL_Renderer* renderer, const vector<string>& paths, const vector<float>& factors) {
        if (paths.size() != factors.size()) {
            std::cerr << "Numarul de cai si factori de parallax trebuie sa fie egal." << std::endl;
            return false;
        }
        for (size_t i = 0; i < paths.size(); i++) {
            SDL_Texture* tex = ResourceManager::getTexture(renderer, paths[i]);
            if (!tex)
                return false;
            layers.push_back(tex);
            parallaxFactors.push_back(factors[i]);
        }
        return true;
    }

    void render(SDL_Renderer* renderer, int cameraX, int cameraY) {
        for (size_t i = 0; i < layers.size(); i++) {
            float factor = parallaxFactors[i];
            int offsetX = static_cast<int>(cameraX * factor);
            int texW, texH;
            SDL_QueryTexture(layers[i], nullptr, nullptr, &texW, &texH);
            int startX = -(offsetX % texW);
            if (startX > 0)
                startX -= texW;
            for (int x = startX; x < SCREEN_WIDTH; x += texW) {
                SDL_Rect destRect = { x, 0, texW, SCREEN_HEIGHT };
                SDL_RenderCopy(renderer, layers[i], nullptr, &destRect);
            }
        }
    }

    void free() {
        layers.clear();
        parallaxFactors.clear();
    }
};

//--------------------------------------------------
// Clasa Platform – reprezintă platformele din joc
//--------------------------------------------------
class Platform {
public:
    SDL_Rect rect;
    Platform(int x, int y, int w, int h) {
        rect = { x, y, w, h };
    }
    void render(SDL_Renderer* renderer, int cameraX) {
        SDL_Rect renderRect = rect;
        renderRect.x -= cameraX;
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &renderRect);
    }
};

//--------------------------------------------------
// Clasa Enemy – implementează animațiile și logica de bază pentru inamici
//--------------------------------------------------
const float ENEMY_SPEED = 100.0f;
const float ENEMY_DETECTION_RANGE = 300.0f;
const float ENEMY_ATTACK_RANGE = 50.0f;

class Enemy {
public:
    float x, y;
    int width, height;
    int health;
    EnemyState currentState;
    unordered_map<EnemyState, AnimationData> animations;
    int currentFrame;
    float animationTimer;
    SDL_Rect rect;
    bool facingRight;

    float patrolMinX, patrolMaxX;
    bool patrolRight = true;


    Enemy(float startX, float startY, int w, int h)
        : x(startX), y(startY), width(w), height(h), health(100),
        currentState(EnemyState::STAND), currentFrame(0), animationTimer(0.0f),
        facingRight(false)
    {
        rect = { static_cast<int>(x), static_cast<int>(y), width, height };
        patrolMinX = x - 100;
        patrolMaxX = x + 100;
    }

    void loadAnimations(SDL_Renderer* renderer) {
        // Stand
        {
            AnimationData data;
            data.texture = ResourceManager::getTexture(renderer, "D:\\PlatformerGame\\Enemies\\enemy1_stand.png");
            data.frameWidth = ENEMY_FRAME_WIDTH;
            data.frameHeight = ENEMY_FRAME_HEIGHT;
            data.totalFrames = 8;
            data.animationSpeed = 0.13f;
            animations[EnemyState::STAND] = data;
        }
        // Walk
        {
            AnimationData data;
            data.texture = ResourceManager::getTexture(renderer, "D:\\PlatformerGame\\Enemies\\enemy1_walk.png");
            data.frameWidth = ENEMY_FRAME_WIDTH;
            data.frameHeight = ENEMY_FRAME_HEIGHT;
            data.totalFrames = 8;
            data.animationSpeed = 0.13f;
            animations[EnemyState::WALK] = data;
        }
        // Attack
        {
            AnimationData data;
            data.texture = ResourceManager::getTexture(renderer, "D:\\PlatformerGame\\Enemies\\enemy1_attack.png");
            data.frameWidth = ENEMY_FRAME_WIDTH;
            data.frameHeight = ENEMY_FRAME_HEIGHT;
            data.totalFrames = 6;
            data.animationSpeed = 0.17f;
            animations[EnemyState::ATTACK] = data;
        }
        // Hurt
        {
            AnimationData data;
            data.texture = ResourceManager::getTexture(renderer, "D:\\PlatformerGame\\Enemies\\enemy1_hurt.png");
            data.frameWidth = ENEMY_FRAME_WIDTH;
            data.frameHeight = ENEMY_FRAME_HEIGHT;
            data.totalFrames = 6;
            data.animationSpeed = 0.17f;
            animations[EnemyState::HURT] = data;
        }
        // Dead
        {
            AnimationData data;
            data.texture = ResourceManager::getTexture(renderer, "D:\\PlatformerGame\\Enemies\\enemy1_dead.png");
            data.frameWidth = ENEMY_FRAME_WIDTH;
            data.frameHeight = ENEMY_FRAME_HEIGHT;
            data.totalFrames = 7;
            data.animationSpeed = 0.14f;
            animations[EnemyState::DEAD] = data;
        }
    }

    void setState(EnemyState newState) {
        if (currentState != newState) {
            currentState = newState;
            currentFrame = 0;
            animationTimer = 0.0f;
        }
    }

    // Update-ul include AI-ul: inamicul se mișcă spre jucător dacă este în raza de detecție
    void update(float deltaTime, float playerX) {
        float distance = playerX - x;
        float absDistance = fabs(distance);

        // DOAR dacă nu e mort
        if (currentState != EnemyState::DEAD && health > 0) {
            if (absDistance < ENEMY_DETECTION_RANGE) {
                if (absDistance < ENEMY_ATTACK_RANGE) {
                    setState(EnemyState::ATTACK);
                }
                else {
                    setState(EnemyState::WALK);

                    if (distance > 0) {
                        x += ENEMY_SPEED * deltaTime;
                        facingRight = true;
                    }
                    else {
                        x -= ENEMY_SPEED * deltaTime;
                        facingRight = false;
                    }
                }
            }
            else {
                // Dacă jucătorul NU e în zonă → patrulare
                setState(EnemyState::WALK);

                if (patrolRight) {
                    x += ENEMY_SPEED * deltaTime;
                    if (x >= patrolMaxX) {
                        x = patrolMaxX;
                        patrolRight = false;
                    }
                }
                else {
                    x -= ENEMY_SPEED * deltaTime;
                    if (x <= patrolMinX) {
                        x = patrolMinX;
                        patrolRight = true;
                    }
                }

                facingRight = patrolRight;
            }
        }

        // Animație
        AnimationData& anim = animations[currentState];
        animationTimer += deltaTime;
        if (animationTimer >= anim.animationSpeed) {
            currentFrame = (currentFrame + 1) % anim.totalFrames;
            animationTimer = 0.0f;
        }

        rect.x = static_cast<int>(x);
        rect.y = static_cast<int>(y);
    }


    void render(SDL_Renderer* renderer, int cameraX) {
        AnimationData& anim = animations[currentState];
        if (!anim.texture) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_Rect fallbackRect = rect;
            fallbackRect.x -= cameraX;
            SDL_RenderFillRect(renderer, &fallbackRect);
            return;
        }
        SDL_Rect srcRect = { currentFrame * anim.frameWidth, 0, anim.frameWidth, anim.frameHeight };
        SDL_Rect dstRect = { rect.x - cameraX, rect.y, width, height };
        SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
        SDL_RenderCopyEx(renderer, anim.texture, &srcRect, &dstRect, 0, nullptr, flip);
    }
};

//--------------------------------------------------
// Clasa Player – gestionează animațiile, mișcarea și logica de atac
//--------------------------------------------------
class Player {
public:
    float x, y;
    int width, height;
    float velocityY;
    bool isJumping;
    bool facingRight;

    bool attackDamageApplied = false;

    unordered_map<PlayerState, AnimationData> animations;
    PlayerState currentState;
    int currentFrame;
    float animationTimer;
    SDL_Rect rect;

    Player(float startX, float startY)
        : x(startX), y(startY), width(80), height(80),
        velocityY(0), isJumping(false), facingRight(true),
        currentState(PlayerState::STAND), currentFrame(0), animationTimer(0.0f)
    {
        rect = { static_cast<int>(x), static_cast<int>(y), width, height };
    }

    void loadAnimations(SDL_Renderer* renderer) {
        // STAND
        {
            AnimationData data;
            data.texture = ResourceManager::getTexture(renderer, "D:\\PlatformerGame\\Character\\player_stand.png");
            data.frameWidth = 115;
            data.frameHeight = 90;
            data.totalFrames = 3;
            data.animationSpeed = 0.34f;
            animations[PlayerState::STAND] = data;
        }
        // WALK
        {
            AnimationData data;
            data.texture = ResourceManager::getTexture(renderer, "D:\\PlatformerGame\\Character\\player_walk.png");
            data.frameWidth = 125;
            data.frameHeight = 90;
            data.totalFrames = 6;
            data.animationSpeed = 0.5f;
            animations[PlayerState::WALK] = data;
        }
        // RUN
        {
            AnimationData data;
            data.texture = ResourceManager::getTexture(renderer, "D:\\PlatformerGame\\Character\\player_run.png");
            data.frameWidth = 95;
            data.frameHeight = 90;
            data.totalFrames = 6;
            data.animationSpeed = 0.17f;
            animations[PlayerState::RUN] = data;
        }
        // JUMP
        {
            AnimationData data;
            data.texture = ResourceManager::getTexture(renderer, "D:\\PlatformerGame\\Character\\player_jump.png");
            data.frameWidth = 80;
            data.frameHeight = 100;
            data.totalFrames = 4;
            data.animationSpeed = 0.25f;
            animations[PlayerState::JUMP] = data;
        }
        // CROUCH
        {
            AnimationData data;
            data.texture = ResourceManager::getTexture(renderer, "D:\\PlatformerGame\\Character\\player_crouch.png");
            data.frameWidth = 87;
            data.frameHeight = 95;
            data.totalFrames = 3;
            data.animationSpeed = 0.34f;
            animations[PlayerState::CROUCH] = data;
        }
        // ATTACK
        {
            AnimationData data;
            data.texture = ResourceManager::getTexture(renderer, "D:\\PlatformerGame\\Character\\player_attack1.png");
            data.frameWidth = 170;
            data.frameHeight = 100;
            data.totalFrames = 3;
            data.animationSpeed = 0.34f;
            animations[PlayerState::ATTACK] = data;
        }
    }

    void setState(PlayerState newState) {
        if (currentState != newState) {
            currentState = newState;
            currentFrame = 0;
            animationTimer = 0.0f;
            if (currentState == PlayerState::ATTACK) {
                attackDamageApplied = false;  // Resetează flag-ul când intrăm în atac
            }
        }
    }


    void handleInput(const Uint8* keystate, float deltaTime) {
        bool movingLeft = keystate[SDL_SCANCODE_LEFT];
        bool movingRight = keystate[SDL_SCANCODE_RIGHT];
        bool crouch = keystate[SDL_SCANCODE_DOWN];
        bool jumpKey = keystate[SDL_SCANCODE_SPACE];
        bool attackKey = keystate[SDL_SCANCODE_E];

        if (attackKey && currentState != PlayerState::ATTACK) {
            setState(PlayerState::ATTACK);
        }
        else if (movingLeft && !movingRight) {
            x -= PLAYER_SPEED * deltaTime;
            facingRight = false;
        }
        else if (movingRight && !movingLeft) {
            x += PLAYER_SPEED * deltaTime;
            facingRight = true;
        }
        if (jumpKey && !isJumping) {
            velocityY = -JUMP_FORCE - height / 2;
            isJumping = true;
            setState(PlayerState::JUMP);
        }
        if (crouch && !isJumping) {
            setState(PlayerState::CROUCH);
        }
        else if (!movingLeft && !movingRight && !isJumping && !crouch && currentState != PlayerState::ATTACK) {
            setState(PlayerState::STAND);
        }
        else if ((movingLeft || movingRight) && !isJumping && !crouch && currentState != PlayerState::ATTACK) {
            setState(PlayerState::RUN);
        }
    }

    void update(float deltaTime, const vector<Platform>& platforms) {
        velocityY += GRAVITY * deltaTime;
        y += velocityY * deltaTime;
        for (const auto& platform : platforms) {
            if (y + height >= platform.rect.y &&
                y + height <= platform.rect.y + 10 &&
                x + width > platform.rect.x &&
                x < platform.rect.x + platform.rect.w &&
                velocityY > 0)
            {
                y = platform.rect.y - height;
                velocityY = 0;
                isJumping = false;
            }
        }
        if (y >= GROUND_LEVEL) {
            y = GROUND_LEVEL;
            velocityY = 0;
            isJumping = false;
            if (currentState == PlayerState::JUMP) {
                setState(PlayerState::STAND);
            }
        }
        updateAnimation(deltaTime);
        rect.x = static_cast<int>(x);
        rect.y = static_cast<int>(y);
    }

    void updateAnimation(float deltaTime) {
        AnimationData& anim = animations[currentState];
        animationTimer += deltaTime;
        if (animationTimer >= anim.animationSpeed) {
            currentFrame = (currentFrame + 1) % anim.totalFrames;
            animationTimer = 0.0f;
            // Când animația de atac se termină (se întoarce la 0) și damage-ul a fost aplicat,
            // trecem la STAND
            if (currentState == PlayerState::ATTACK && currentFrame == 0) {
                setState(PlayerState::STAND);
            }
        }
    }

    SDL_Rect getAttackHitbox() {
        SDL_Rect hitbox;
        if (facingRight) {
            hitbox.x = rect.x + rect.w;
        }
        else {
            hitbox.x = rect.x - 30;
        }
        hitbox.y = rect.y + rect.h / 4;
        hitbox.w = 30;
        hitbox.h = rect.h / 2;
        return hitbox;
    }

    void render(SDL_Renderer* renderer, int cameraX) {
        AnimationData& anim = animations[currentState];
        if (!anim.texture) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_Rect fallbackRect = rect;
            fallbackRect.x -= cameraX;
            SDL_RenderFillRect(renderer, &fallbackRect);
            return;
        }
        SDL_Rect srcRect = { currentFrame * anim.frameWidth, 0, anim.frameWidth, anim.frameHeight };
        SDL_Rect dstRect = { rect.x - cameraX, rect.y, width, height };
        SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
        SDL_RenderCopyEx(renderer, anim.texture, &srcRect, &dstRect, 0, nullptr, flip);
    }
};

int main(int argc, char* argv[]) {
    Renderer renderer;
    if (!renderer.init()) {
        return -1;
    }

    // Inițializare font pentru HUD
    TTF_Font* font = TTF_OpenFont("D:\\PlatformerGame\\Fonts\\PressStart2P-Regular.ttf", 24);
    if (!font) {
        std::cerr << "Eroare la deschiderea fontului: " << TTF_GetError() << std::endl;
        return -1;
    }

    // Inițializare fundal multi-layer
    MultiLayerBackground background;
    vector<string> bgPaths = {
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
    vector<float> bgFactors = { 0.1f, 0.2f, 0.3f, 0.3f, 0.4f, 0.5f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f };
    if (!background.load(renderer.sdlRenderer, bgPaths, bgFactors)) {
        std::cerr << "Eroare la incarcarea fundalului multi-layer." << std::endl;
    }

    // Inițializare jucător și animații
    Player player(100, 500);
    player.loadAnimations(renderer.sdlRenderer);

    // Inițializare platforme
    vector<Platform> platforms = {
        {200, 550, 200, 20},
        {450, 500, 200, 20},
        {700, 450, 200, 20},
        {950, 400, 200, 20},
        {300, 350, 200, 20}
    };

    // Inițializare inamici
    vector<Enemy> enemies;
    Enemy enemy1(800, 590, ENEMY_FRAME_WIDTH, ENEMY_FRAME_HEIGHT);
    enemy1.loadAnimations(renderer.sdlRenderer);
    enemies.push_back(enemy1);
    Enemy enemy2(1200, 590, ENEMY_FRAME_WIDTH, ENEMY_FRAME_HEIGHT);
    enemy2.loadAnimations(renderer.sdlRenderer);
    enemies.push_back(enemy2);

    // Variabile pentru HUD (ex: health și score)
    int playerHealth = 100;
    int score = 0;

    bool isRunning = true;
    SDL_Event event;
    Uint32 lastTime = SDL_GetTicks();
    while (isRunning) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                isRunning = false;
        }
        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        player.handleInput(keystate, deltaTime);
        player.update(deltaTime, platforms);
        for (auto& enemy : enemies) {
            enemy.update(deltaTime, player.x);
        }

        if (player.currentState == PlayerState::ATTACK && player.currentFrame == 1 && !player.attackDamageApplied) {
            SDL_Rect attackHitbox = player.getAttackHitbox();
            for (auto& enemy : enemies) {
                if (SDL_HasIntersection(&attackHitbox, &enemy.rect)) {
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


        enemies.erase(remove_if(enemies.begin(), enemies.end(),
            [](const Enemy& e) { return e.health <= 0; }), enemies.end());
        int cameraX = static_cast<int>(player.x) - SCREEN_WIDTH / 2;
        renderer.clear();
        background.render(renderer.sdlRenderer, cameraX, 0);
        for (auto& platform : platforms) {
            platform.render(renderer.sdlRenderer, cameraX);
        }
        for (auto& enemy : enemies) {
            enemy.render(renderer.sdlRenderer, cameraX);
        }
        player.render(renderer.sdlRenderer, cameraX);
        // Randăm HUD-ul în partea de sus a ecranului
        renderHUD(renderer.sdlRenderer, font, playerHealth, score);
        renderer.present();
    }
    background.free();
    TTF_CloseFont(font);
    renderer.close();
    return 0;
}
