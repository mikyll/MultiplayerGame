#pragma once

#include "common.h"

#include "enet/enet.h"

// Init
extern void initSDL(void);
extern void cleanupSDL(void);

// Networking
extern void initEnet(void);
extern void cleanupEnet(void);

// Input
extern void doInput(void);

// Draw
extern void prepareScene(void);
extern void presentScene(void);
extern void blitRect(int x, int y, int w, int h, SDL_Color c);
extern void blitRectBorder(int x, int y, int w, int h, SDL_Color c);

// Text
extern void initFonts(void);

// Menu
extern void initMenu(void);
extern void showMenu(void);

// Game
extern void startGame(void);
extern Game game;

// UI
extern void drawFPS(int fps);

extern App app;

