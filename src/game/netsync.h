#pragma once

#include "../core/common.h"


void initNetworkSync(void);
void sendGameUpdate(void);


// Player
extern int spawnPlayer(Entity p);
extern int spawnPlayerAt(float x, float y);
extern void destroyPlayerByID(int id);
extern Entity* getPlayerByID(int id);
extern void setLocalPlayer(Entity* player);
extern Entity* getLocalPlayer(void);

// Networking
extern int getHostType(void);
extern void setServerHandlers(const ServerHandlers* serverHandlers);
extern void serverSend(int client, const NetMessage* message);
extern void serverBroadcast(const NetMessage* message);
extern int serverClientsCount(void);
extern void setClientHandlers(const ClientHandlers* clientHandlers);
extern void clientSend(const NetMessage* message);

extern Game game;
