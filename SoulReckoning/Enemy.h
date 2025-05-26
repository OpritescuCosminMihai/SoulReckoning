// Enemy.h
#pragma once
#include <SDL.h>
#include <unordered_map>
#include <string>
#include "Globals.h"
#include "AnimationData.h"
#include "ResourceManager.h"

class Enemy {
public:
    float x, y;// Poziția inamicului în lume
    int width, height;// Dimensiunile vizuale ale inamicului
    float knockbackSpeed = 0.0f;// Viteză de knockback aplicată când e lovit
    float knockbackTimer = 0.0f;// Durata pentru care knockback-ul se aplică
    int health;// Sănătatea inamicului (scade când este atacat)
    EnemyState currentState;// Starea curentă a inamicului (ex: WALK, ATTACK, DEAD)
    std::unordered_map<EnemyState, AnimationData> animations;// Dicționar cu animații pentru fiecare stare posibilă
    int currentFrame;// Cadru curent din animație
    float animationTimer;// Timpul scurs de la ultimul cadru – pentru redare fluentă
    SDL_Rect rect;// Rect pentru coliziuni și desenare
    bool facingRight;// Direcția în care este orientat sprite-ul
    bool hasDealtDamage = false;// Dacă a aplicat deja damage în cadrul atacului curent
    Uint32 lastAttackTime = 0;// Timpul ultimei încercări de atac (pentru cooldown)

    float patrolMinX, patrolMaxX;// Limite pentru mișcarea de patrulare
    bool patrolRight = true;// Direcția de patrulare (true = dreapta)

    Enemy(float startX, float startY, int w, int h);// Constructor: setează poziția, dimensiunile și starea inițială

    void loadAnimations(SDL_Renderer* renderer); // Încarcă animațiile inamicului pentru fiecare stare (stand, walk, etc.)
    void setState(EnemyState newState);// Schimbă starea inamicului și resetează animația
    void update(float deltaTime, float playerX);// Actualizează logica: AI, patrulare, atacuri, knockback, animații
    void render(SDL_Renderer* renderer, int cameraX);// Desenează inamicul pe ecran, ținând cont de camera
};
