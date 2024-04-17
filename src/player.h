#pragma once

#include "common.h"

// Draw
extern void blitRect(int x, int y, int w, int h, SDL_Color c);
extern void blitRectBorder(int x, int y, int w, int h, SDL_Color c);

// Text
extern void drawText(int x, int y, int r, int g, int b, int align, char* format, ...);
extern void drawTextScaled(int x, int y, float size, int r, int g, int b, int align, char* format, ...);

extern App app;
extern Game game;