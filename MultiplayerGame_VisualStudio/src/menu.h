#pragma once

#include "common.h"

// Draw
extern void blitRect(int x, int y, int w, int h, SDL_Color c);

// Text
extern void drawText(int x, int y, int r, int g, int b, int align, char* format, ...);
extern void drawTextScaled(int x, int y, float size, int r, int g, int b, int align, char* format, ...);

// Game
extern void initGame(void);
extern void showGame(void);

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

// Utils
extern void secure_strcpy(char* dest, size_t destSize, const char* src);

extern App app;
