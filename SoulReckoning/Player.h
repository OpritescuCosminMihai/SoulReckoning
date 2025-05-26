// Player.h
#pragma once
#include <SDL.h>
#include <unordered_map>
#include <vector>
#include <string>
#include "Globals.h"
#include "AnimationData.h"
#include "Platform.h"

class Player {
public:
    float x, y;// Poziția jucătorului pe axele X și Y
    int width, height;// Dimensiunile vizuale ale jucătorului
    float velocityY;// Viteza pe axa verticală (folosită pentru gravitație/sărituri)
    bool isJumping;// Indică dacă jucătorul este în aer (sărind sau căzând)
    bool facingRight;// Indică direcția în care este orientat jucătorul
    bool attackDamageApplied;// Asigură că damage-ul e aplicat o singură dată per atac
    // Knockback – viteză orizontală și verticală când jucătorul este lovit
    float knockbackSpeedX = 0.0f;
    float knockbackSpeedY = 0.0f;
    float knockbackDuration = 0.0f;// Durata rămasă a efectului de knockback
    bool beingKnockedBack = false;// Indică dacă jucătorul este în starea de knockback

    std::unordered_map<PlayerState, AnimationData> animations;// Dicționar care asociază fiecare stare cu animația aferentă
    PlayerState currentState;// Starea curentă a jucătorului (ex: STAND, RUN, ATTACK)
    int currentFrame;// Cadru curent al animației (pentru redare secvențială)
    float animationTimer;// Cronometru pentru schimbarea cadrelor animației
    SDL_Rect rect;// Rect pentru coliziuni și desenare

    Player(float startX, float startY);// Constructor – inițializează poziția și dimensiunile de start

    void loadAnimations(SDL_Renderer* renderer);// Încarcă toate sprite-urile și parametrii animațiilor
    void setState(PlayerState newState);// Schimbă starea jucătorului și resetează animația dacă e cazul
    void handleInput(const Uint8* keystate, float deltaTime);// Procesează input-ul tastaturii și schimbă comportamentul jucătorului
    void update(float deltaTime, const std::vector<Platform>& platforms);// Actualizează fizica (gravitația, coliziunile) și starea jucătorului
    void updateAnimation(float deltaTime);// Actualizează animația curentă în funcție de timp
    SDL_Rect getAttackHitbox();// Returnează zona în care atacul jucătorului produce damage
    void render(SDL_Renderer* renderer, int cameraX);// Desenează jucătorul pe ecran, ținând cont de poziția camerei
    void reset(); // Resetează jucătorul la poziția de start (ex: după moarte)
    SDL_Rect getCollider() const;// Returnează colliderul general al jucătorului pentru coliziuni
};

