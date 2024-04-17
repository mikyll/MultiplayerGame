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
extern void doPlayers();
extern void drawPlayers();

// Networking
extern int getHostType();
extern char* hostTypeToString(int type);
extern void disposeHost();
extern void doNetworkingBefore();
extern void doNetworkingAfter();

extern App app;