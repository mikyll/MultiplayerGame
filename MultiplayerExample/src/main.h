#pragma once

#include "common.h"

#include "enet/enet.h"

// Init
extern void initSDL();
extern void cleanupSDL();

// Networking
extern void initEnet();
extern void cleanupEnet();

// Input
extern void doInput();

// Draw
extern void prepareScene();
extern void presentScene();
extern void blitRect(int x, int y, int w, int h, SDL_Color c);
extern void blitRectBorder(int x, int y, int w, int h, SDL_Color c);

// Text
extern void initFonts();
extern void drawTextScaled(int x, int y, float size, int r, int g, int b, int align, char* format, ...);

// Menu
extern void showMenu();

// Game
extern void showGame();
extern Game game;

App app;