#pragma once

#include "../common.h"

#include "enet/enet.h"

// Player
extern Entity* getLocalPlayer();
extern void setLocalPlayer(Entity* player);
extern int spawnPlayerAt(float x, float y);
extern void destroyPlayerByID(int id);
extern Entity* getPlayerByID(int id);

// Utils
extern int isValidIPAddress(const char* ipAddress);