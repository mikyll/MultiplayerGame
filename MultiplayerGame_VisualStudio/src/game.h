#pragma once

#include "common.h"

// Menu
extern void showMenu();

// Text
extern void drawText(int x, int y, int r, int g, int b, int align, char* format, ...);
extern void drawTextScaled(int x, int y, float size, int r, int g, int b, int align, char* format, ...);

// Player
extern int spawnPlayerAt(float x, float y);
extern Entity* getPlayerByID(int id);
extern void setLocalPlayer(Entity* player);
extern void doPlayers(float deltaTime);
extern void drawPlayers();

// Networking
extern void setHostType(int type);
extern int getHostType();
extern char* hostTypeToString(int type);
extern void netDisposeHost();
extern void doNetworkingBefore();
extern void doNetworkingAfter();

// Utils
extern int secure_sprintf(char* buffer, size_t bufferSize, const char* format, ...);

extern App app;