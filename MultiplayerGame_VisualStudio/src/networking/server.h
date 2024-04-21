#pragma once

#include "../common.h"

#include "enet/enet.h"


void setServerOnConnect(void (*onConnectOk)(void));
void setServerOnDisconnect(void (*onDisconnect)(void));
int createServer(char* ipAddress, int port);
void destroyServer(void);
void serverBefore(void);
void serverAfter(void);


// Player
extern void setLocalPlayer(Entity* player);
extern Entity* getPlayerByID(int id);
extern int spawnPlayerAt(float x, float y);
extern void destroyPlayerByID(int id);

// Utils
extern int isValidIPAddress(const char* ipAddress);
extern void secure_strcpy(char* dest, size_t destSize, const char* src);

extern Game game;
