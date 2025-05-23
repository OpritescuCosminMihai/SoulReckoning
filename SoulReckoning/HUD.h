// HUD.h
#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

void renderHUD(SDL_Renderer* renderer, TTF_Font* font, int playerHealth, int score);
