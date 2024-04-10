#pragma once

#include "common.h"

int createPlayer(float x, float y);
void doPlayers();
void drawPlayers();

// Draw
extern void blitRect(int x, int y, int w, int h, SDL_Color c);
extern void blitRectBorder(int x, int y, int w, int h, SDL_Color c);

extern App app;
extern Game game;