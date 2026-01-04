#pragma once

#include "../common.h"

typedef struct Button Button;
struct Button {
  int x, y;
  int w, h;
  SDL_Color c;
  char text[MAX_BUTTON_TEXT_LENGTH];
  SDL_Color textC;
  void (*onOver)(void);
  void (*onOut)(void);
  void (*onPress)(void);
  void (*onRelease)(void);
  Button* next;
};


// Draw
extern void blitRect(int x, int y, int w, int h, SDL_Color c);

// Text
extern void drawText(int x, int y, int r, int g, int b, int align, char* format, ...);
extern void drawTextScaled(int x, int y, float size, int r, int g, int b, int align, char* format, ...);

// Utils
extern void secure_strcpy(char* dest, size_t destSize, const char* src);
