#pragma once

#include "common.h"

#include "enet/enet.h"

// Player
extern int spawnPlayerAt(float x, float y);
extern void destroyPlayerByID(int id);
extern void setLocalPlayer(Entity* player);
extern Entity* getPlayerByID(int id);

extern Game game;