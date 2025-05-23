// Player.cpp
#include "Player.h"
#include "ResourceManager.h"
#include "AnimationData.h"


Player::Player(float startX, float startY)
    : x(startX), y(startY), width(80), height(80),
    velocityY(0), isJumping(false), facingRight(true),
    attackDamageApplied(false), currentState(PlayerState::STAND),
    currentFrame(0), animationTimer(0.0f)
{
    rect = { static_cast<int>(x), static_cast<int>(y), width, height };
}

void Player::loadAnimations(SDL_Renderer* renderer) {
    // STAND
    AnimationData stand;
    stand.texture = ResourceManager::getTexture(renderer, "D:\\PlatformerGame\\Character\\player_stand.png");
    stand.frameWidth = 115;
    stand.frameHeight = 90;
    stand.totalFrames = 3;
    stand.animationSpeed = 0.34f;
    animations[PlayerState::STAND] = stand;

    // WALK
    AnimationData walk;
    walk.texture = ResourceManager::getTexture(renderer, "D:\\PlatformerGame\\Character\\player_walk.png");
    walk.frameWidth = 125;
    walk.frameHeight = 90;
    walk.totalFrames = 6;
    walk.animationSpeed = 0.5f;
    animations[PlayerState::WALK] = walk;

    // RUN
    AnimationData run;
    run.texture = ResourceManager::getTexture(renderer, "D:\\PlatformerGame\\Character\\player_run.png");
    run.frameWidth = 95;
    run.frameHeight = 90;
    run.totalFrames = 6;
    run.animationSpeed = 0.17f;
    animations[PlayerState::RUN] = run;

    // JUMP
    AnimationData jump;
    jump.texture = ResourceManager::getTexture(renderer, "D:\\PlatformerGame\\Character\\player_jump.png");
    jump.frameWidth = 80;
    jump.frameHeight = 100;
    jump.totalFrames = 4;
    jump.animationSpeed = 0.25f;
    animations[PlayerState::JUMP] = jump;

    // CROUCH
    AnimationData crouch;
    crouch.texture = ResourceManager::getTexture(renderer, "D:\\PlatformerGame\\Character\\player_crouch.png");
    crouch.frameWidth = 87;
    crouch.frameHeight = 95;
    crouch.totalFrames = 3;
    crouch.animationSpeed = 0.34f;
    animations[PlayerState::CROUCH] = crouch;

    // ATTACK
    AnimationData attack;
    attack.texture = ResourceManager::getTexture(renderer, "D:\\PlatformerGame\\Character\\player_attack1.png");
    attack.frameWidth = 170;
    attack.frameHeight = 100;
    attack.totalFrames = 3;
    attack.animationSpeed = 0.34f;
    animations[PlayerState::ATTACK] = attack;
}

void Player::setState(PlayerState newState) {
    if (currentState != newState) {
        currentState = newState;
        currentFrame = 0;
        animationTimer = 0.0f;
        if (currentState == PlayerState::ATTACK) {
            attackDamageApplied = false;
        }
    }
}

void Player::handleInput(const Uint8* keystate, float deltaTime) {
    bool left = keystate[SDL_SCANCODE_LEFT];
    bool right = keystate[SDL_SCANCODE_RIGHT];
    bool down = keystate[SDL_SCANCODE_DOWN];
    bool jump = keystate[SDL_SCANCODE_SPACE];
    bool attack = keystate[SDL_SCANCODE_E];

    if (attack && currentState != PlayerState::ATTACK) {
        setState(PlayerState::ATTACK);
    }
    else if (left && !right) {
        x -= PLAYER_SPEED * deltaTime;
        facingRight = false;
    }
    else if (right && !left) {
        x += PLAYER_SPEED * deltaTime;
        facingRight = true;
    }

    if (jump && !isJumping) {
        velocityY = -JUMP_FORCE - height / 2;
        isJumping = true;
        setState(PlayerState::JUMP);
    }

    if (down && !isJumping) {
        setState(PlayerState::CROUCH);
    }
    else if (!left && !right && !isJumping && !down && currentState != PlayerState::ATTACK) {
        setState(PlayerState::STAND);
    }
    else if ((left || right) && !isJumping && !down && currentState != PlayerState::ATTACK) {
        setState(PlayerState::RUN);
    }
}

void Player::update(float deltaTime, const std::vector<Platform>& platforms) {
    velocityY += GRAVITY * deltaTime;
    y += velocityY * deltaTime;

    for (const auto& p : platforms) {
        if (y + height >= p.rect.y &&
            y + height <= p.rect.y + 10 &&
            x + width > p.rect.x &&
            x < p.rect.x + p.rect.w &&
            velocityY > 0)
        {
            y = p.rect.y - height;
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

void Player::updateAnimation(float deltaTime) {
    AnimationData& anim = animations[currentState];
    animationTimer += deltaTime;
    if (animationTimer >= anim.animationSpeed) {
        currentFrame = (currentFrame + 1) % anim.totalFrames;
        animationTimer = 0.0f;

        if (currentState == PlayerState::ATTACK && currentFrame == 0) {
            setState(PlayerState::STAND);
        }
    }
}

SDL_Rect Player::getAttackHitbox() {
    SDL_Rect hitbox;
    hitbox.x = facingRight ? rect.x + rect.w : rect.x - 30;
    hitbox.y = rect.y + rect.h / 4;
    hitbox.w = 30;
    hitbox.h = rect.h / 2;
    return hitbox;
}

void Player::render(SDL_Renderer* renderer, int cameraX) {
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
