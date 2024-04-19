#pragma once

#include "common.h"

// Draw
extern void blitRect(int x, int y, int w, int h, SDL_Color c);

// Text
extern void drawText(int x, int y, int r, int g, int b, int align, char* format, ...);
extern void drawTextScaled(int x, int y, float size, int r, int g, int b, int align, char* format, ...);

// Game
extern void initGame();
extern void showGame();

// Networking
extern void setHostType(int type);
extern int createHost();
extern void disposeHost();

// UI
extern void doTextInput();
extern void drawTextInput();

// Utils
extern void secure_strcpy(char* dest, size_t destSize, const char* src);

extern App app;