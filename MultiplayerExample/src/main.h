#pragma once

#include "common.h"

// Init
extern void initSDL();
extern void cleanup();

// Input
extern void doInput();

// Draw
extern void prepareScene();
extern void presentScene();
extern void blitRect(int x, int y, int w, int h, SDL_Color c);
extern void blitRectBorder(int x, int y, int w, int h, SDL_Color c);

// Game
extern void resetGame();
extern Game game;

App app;