#pragma once

#include "common.h"
#include "ui/button.h"

// Draw
extern void blitRect(int x, int y, int w, int h, SDL_Color c);

// Text
extern void drawText(int x, int y, int r, int g, int b, int align, char* format, ...);
extern void drawTextScaled(int x, int y, float size, int r, int g, int b, int align, char* format, ...);

// Game
extern void initGame(void);
extern void startGame(void);

// Networking
extern void setHostType(int type);
extern int netCreateHost(char* ipAddress, int port);
extern void setAfterConnect(void (*callbackOk)(void), void (*callbackFail)(void));
extern void setAfterDisconnect(void (*callback)(void));
extern void netDisposeHost(void);
extern void doNetworkingBefore(void);
extern void doNetworkingAfter(void);

// UI
extern void doTextInput(void);
extern void drawTextInput(void);
extern char* getTextInput(void);

// Button
extern Button createButton(
  int posX, int posY, int width, int height, SDL_Color color,
  char* text, SDL_Color textColor,
  void (*onOver)(void), void (*onOut)(void),
  void (*onPress)(void), void (*onRelease)(void));
extern void updateButtonText(Button* button, const char* format, ...);
extern void updateButtonColor(Button* button, SDL_Color background, SDL_Color text);
extern void drawButton(Button button);

// Utils
extern void secure_strcpy(char* dest, size_t destSize, const char* src);
extern int isLight(SDL_Color c);
extern unsigned int hexFromRGB(int r, int g, int b);
extern SDL_Color getRandomColor();

extern App app;

