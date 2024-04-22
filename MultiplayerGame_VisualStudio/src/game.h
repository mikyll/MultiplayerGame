#pragma once

#include "common.h"


void initGame(void);
void showGame(void);


// Menu
extern void showMenu(void);

// Text
extern void drawText(int x, int y, int r, int g, int b, int align, char* format, ...);
extern void drawTextScaled(int x, int y, float size, int r, int g, int b, int align, char* format, ...);

// Player
extern int spawnPlayerAt(float x, float y);
extern Entity* getPlayerByID(int id);
extern void setLocalPlayer(Entity* player);
extern void doPlayers(float deltaTime);
extern void drawPlayers(void);

// Networking
extern void setHostType(int type);
extern int getHostType(void);
extern char* hostTypeToString(int type);
extern void netDisposeHost(void);
extern void doNetworkingBefore(void);
extern void doNetworkingAfter(void);

// Utils
extern int secure_sprintf(char* buffer, size_t bufferSize, const char* format, ...);

extern App app;
extern Game game;
