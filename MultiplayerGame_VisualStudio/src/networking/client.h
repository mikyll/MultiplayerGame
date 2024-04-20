#pragma once

#include "../common.h"

#include "enet/enet.h"

// Player
extern Entity* getLocalPlayer();
extern void setLocalPlayer(Entity* player);
extern int spawnPlayer(Entity p);
extern void destroyPlayerByID(int id);
extern Entity* getPlayerByID(int id);

// Utils
extern int isValidIPAddress(const char* ipAddress);