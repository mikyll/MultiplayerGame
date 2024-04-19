#pragma once

#include "../common.h"

#include "enet/enet.h"

// Player
extern int spawnPlayerAt(float x, float y);
extern void destroyPlayerByID(int id);
extern void setLocalPlayer(Entity* player);
extern Entity* getPlayerByID(int id);

// Utils
extern void secure_strcpy(char* dest, size_t destSize, const char* src);

extern Game game;