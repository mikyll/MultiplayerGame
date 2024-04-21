#pragma once

#include "common.h"

#include "SDL2/SDL_image.h"

void prepareScene(void);
void presentScene(void);
void blitRect(int x, int y, int w, int h, SDL_Color c);
void blitRectBorder(int x, int y, int w, int h, SDL_Color c);

extern App app;
