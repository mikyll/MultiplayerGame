#pragma once

#include "../common.h"

#include "enet/enet.h"

// Server
extern void setServerOnConnect(void (*onConnect)(void));
extern void setServerOnDisconnect(void (*onDisconnect)(void));
extern int createServer(char* ipAddress, int port);
extern void destroyServer();
extern void serverBefore();
extern void serverAfter();

// Client
extern void setClientOnConnect(void (*onConnectOk)(void), void (*onConnectFail)(void));
extern void setClientOnDisconnect(void (*onDisconnect)(void));
extern int createClient(char* ipAddress, int port);
extern void destroyClient();
extern void clientBefore();
extern void clientAfter();


void initEnet();
void cleanupEnet();

extern Game game;
