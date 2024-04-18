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

// Menu
extern void showMenu();

// Game
extern void showGame();
extern Game game;

// UI
extern void drawFPS(int fps);

App app;