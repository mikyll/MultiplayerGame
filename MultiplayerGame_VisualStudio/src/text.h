#pragma once

#include "common.h"

// Draw
extern void blitRectTexture(SDL_Texture* texture, SDL_Rect* src, int x, int y);
extern void blitRectScaledTexture(SDL_Texture* texture, SDL_Rect* src, int x, int y, float size);
extern SDL_Texture* loadTexture(char* filename);

// Utils
extern int secure_sprintf(char* buffer, size_t bufferSize, const char* format, ...);