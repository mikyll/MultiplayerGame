#pragma once

#include "../core/common.h"


void initNetworkSync(void);
void sendGameUpdate(void);


// Player
extern int spawnPlayerAt(float x, float y);
extern void destroyPlayerByID(int id);
extern Entity* getPlayerByID(int id);
extern void setLocalPlayer(Entity* player);

// Networking
extern int getHostType(void);
extern void setServerHandlers(const ServerHandlers* serverHandlers);
extern void serverSend(int client, const void* data, size_t size);
extern void serverBroadcast(const void* data, size_t size);
extern int serverClientsCount(void);

extern Game game;
