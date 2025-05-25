// Enemy.cpp
#include "Enemy.h"

Enemy::Enemy(float startX, float startY, int w, int h)
    : x(startX), y(startY), width(w), height(h), health(100),
    currentState(EnemyState::STAND), currentFrame(0), animationTimer(0.0f),
    facingRight(false)
{
    rect = { static_cast<int>(x), static_cast<int>(y), width, height };
    patrolMinX = x - 100;
    patrolMaxX = x + 100;
}

void Enemy::loadAnimations(SDL_Renderer* renderer) {
    auto load = [&](EnemyState state, const std::string& path, int frames, float speed) {
        AnimationData data;
        data.texture = ResourceManager::getTexture(renderer, path);
        data.frameWidth = ENEMY_FRAME_WIDTH;
        data.frameHeight = ENEMY_FRAME_HEIGHT;
        data.totalFrames = frames;
        data.animationSpeed = speed;
        animations[state] = data;
        };

    load(EnemyState::STAND, "D:\\PlatformerGame\\Enemies\\enemy1_stand.png", 8, 0.13f);
    load(EnemyState::WALK, "D:\\PlatformerGame\\Enemies\\enemy1_walk.png", 8, 0.13f);
    load(EnemyState::ATTACK, "D:\\PlatformerGame\\Enemies\\enemy1_attack.png", 6, 0.17f);
    load(EnemyState::HURT, "D:\\PlatformerGame\\Enemies\\enemy1_hurt.png", 6, 0.2f);
    load(EnemyState::DEAD, "D:\\PlatformerGame\\Enemies\\enemy1_dead.png", 7, 0.14f);
}

void Enemy::setState(EnemyState newState) {
    if (currentState != newState) {
        currentState = newState;
        currentFrame = 0;
        animationTimer = 0.0f;
    }
}

void Enemy::update(float deltaTime, float playerX) {
    float distance = playerX - x;
    float absDistance = fabs(distance);

    // 📌 Dacă e mort, joacă animatia și rămâne pe ultimul frame
    if (currentState == EnemyState::DEAD) {
        AnimationData& anim = animations[currentState];
        animationTimer += deltaTime;

        if (animationTimer >= anim.animationSpeed) {
            currentFrame = std::min(currentFrame + 1, anim.totalFrames - 1);
            animationTimer = 0.0f;
        }

        rect.x = static_cast<int>(x);
        rect.y = static_cast<int>(y);
        return;
    }

    // 📌 Dacă e în knockback, continuă animatia HURT și aplică deplasarea
    if (knockbackTimer > 0.0f) {
        x += knockbackSpeed * deltaTime;
        knockbackTimer -= deltaTime;

        if (currentState != EnemyState::HURT) {
            setState(EnemyState::HURT);
        }

        AnimationData& anim = animations[EnemyState::HURT];
        animationTimer += deltaTime;
        if (animationTimer >= anim.animationSpeed) {
            currentFrame = (currentFrame + 1) % anim.totalFrames;
            animationTimer = 0.0f;
        }

        rect.x = static_cast<int>(x);
        rect.y = static_cast<int>(y);
        return;
    }

    // 📌 Dacă atacă, termină animatia înainte de orice logică
    if (currentState == EnemyState::ATTACK) {
        AnimationData& anim = animations[currentState];
        animationTimer += deltaTime;

        if (animationTimer >= anim.animationSpeed) {
            currentFrame++;
            animationTimer = 0.0f;

            if (currentFrame >= anim.totalFrames) {
                currentFrame = 0;
                hasDealtDamage = false;
                setState(EnemyState::STAND);
            }
        }

        rect.x = static_cast<int>(x);
        rect.y = static_cast<int>(y);
        return;
    }

    // 📌 Comportament AI: urmărire sau patrulare
    if (absDistance < ENEMY_DETECTION_RANGE) {
        if (absDistance < ENEMY_ATTACK_RANGE) {
            Uint32 now = SDL_GetTicks();
            if (now - lastAttackTime > 2000) {
                setState(EnemyState::ATTACK);
                currentFrame = 0;
                animationTimer = 0.0f;
                lastAttackTime = now;
                hasDealtDamage = false;
            }
            else {
                setState(EnemyState::STAND);
            }
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

    // 📌 Animatie normală (WALK, STAND etc.)
    AnimationData& anim = animations[currentState];
    animationTimer += deltaTime;
    if (animationTimer >= anim.animationSpeed) {
        currentFrame = (currentFrame + 1) % anim.totalFrames;
        animationTimer = 0.0f;
    }

    rect.x = static_cast<int>(x);
    rect.y = static_cast<int>(y);
}

void Enemy::render(SDL_Renderer* renderer, int cameraX) {
    rect.x = static_cast<int>(x);
    rect.y = static_cast<int>(y);

    AnimationData& anim = animations[currentState];
    if (!anim.texture) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect fallback = rect;
        fallback.x -= cameraX;
        SDL_RenderFillRect(renderer, &fallback);
        return;
    }

    SDL_Rect src = { currentFrame * anim.frameWidth, 0, anim.frameWidth, anim.frameHeight };
    SDL_Rect dst = { rect.x - cameraX, rect.y, width, height };
    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(renderer, anim.texture, &src, &dst, 0, nullptr, flip);
}