#pragma once

#include "../common.h"


int findFirstAvailableID(void);
int spawnPlayer(Entity p);
int spawnPlayerAt(float x, float y);
void destroyPlayer(Entity* player);
void destroyPlayerByID(int id);
void setLocalPlayer(Entity* player);
Entity* getPlayerByID(int id);
Entity* getLocalPlayer(void);
Entity* getPlayerByID(int id);
void doPlayers(float deltaTime);
void drawPlayers(void);


// Draw
extern void blitRect(int x, int y, int w, int h, SDL_Color c);
extern void blitRectBorder(int x, int y, int w, int h, SDL_Color c);

// Text
extern void drawText(int x, int y, int r, int g, int b, int align, char* format, ...);
extern void drawTextScaled(int x, int y, float size, int r, int g, int b, int align, char* format, ...);

// Utils
extern int secure_sprintf(char* buffer, size_t bufferSize, const char* format, ...);

extern App app;
extern Game game;
