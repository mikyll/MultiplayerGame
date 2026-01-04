#pragma once

#include "../common.h"

#include "enet/enet.h"


void setClientOnConnect(void (*onConnectOk)(void), void (*onConnectFail)(void));
void setClientOnDisconnect(void (*onDisconnect)(void));
int createClient(char* ipAddress, int port);
void destroyClient(void);
void clientBefore(void);
void clientAfter(void);


// Player
extern Entity* getLocalPlayer(void);
extern void setLocalPlayer(Entity* player);
extern int spawnPlayer(Entity p);
extern void destroyPlayerByID(int id);
extern Entity* getPlayerByID(int id);

// Utils
extern int isValidIPAddress(const char* ipAddress);

