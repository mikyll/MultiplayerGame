#pragma once

#include "../common.h"


void initFonts(void);
void drawText(int x, int y, int r, int g, int b, int align, char* format, ...);
void drawTextScaled(int x, int y, float size, int r, int g, int b, int align, char* format, ...);


// Draw
extern void blitRectTexture(SDL_Texture* texture, SDL_Rect* src, int x, int y);
extern void blitRectScaledTexture(SDL_Texture* texture, SDL_Rect* src, int x, int y, float size);
extern SDL_Texture* loadTexture(char* filename);

// Utils
extern int secure_sprintf(char* buffer, size_t bufferSize, const char* format, ...);

