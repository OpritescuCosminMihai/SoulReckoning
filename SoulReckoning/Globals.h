#pragma once
#include <SDL.h>

// =====================
// Constante globale
// =====================

// Dimensiunile ferestrei jocului
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

// Y-ul care reprezintă nivelul solului (folosit pentru player, inamici, etc.)
const int GROUND_LEVEL = 600;

// Viteza de deplasare a jucătorului (pixeli/secundă)
const float PLAYER_SPEED = 300.0f;

// Gravitația aplicată asupra jucătorului (accelerație în pixeli/sec^2)
const float GRAVITY = 1000.0f;

// Forța de săritură a jucătorului (viteza inițială în sus la săritură)
const float JUMP_FORCE = 500.0f;

// Dimensiuni implicite pentru fiecare frame din sprite-urile inamicilor
const int ENEMY_FRAME_WIDTH = 85;
const int ENEMY_FRAME_HEIGHT = 90;

// Viteza cu care se deplasează inamicii (pixeli/secundă)
const float ENEMY_SPEED = 100.0f;

// Distanța la care inamicul „vede” jucătorul și începe să-l urmărească
const float ENEMY_DETECTION_RANGE = 300.0f;

// Distanța la care inamicul se oprește și declanșează un atac
const float ENEMY_ATTACK_RANGE = 50.0f;

// =====================
// Enum-uri globale
// =====================

// Stările posibile ale jucătorului (folosite pentru animații și logică)
enum class PlayerState {
    STAND,   // Stă pe loc
    WALK,    // Merge încet
    RUN,     // Aleargă
    JUMP,    // Sare
    CROUCH,  // Se lasă jos
    ATTACK,  // Atacă
    DEAD,    // Moare
    BLOCK    // Se apără
};

// Stările posibile ale inamicilor
enum class EnemyState {
    STAND,  // Stau pe loc
    WALK,   // Patrulează
    ATTACK, // Atacă jucătorul
    HURT,   // Sunt loviți
    DEAD    // Mor și dispar
};
