#pragma once

#include "../common.h"

#include "enet/enet.h"

// Server
extern int createServer(char* ipAddress, int port);
extern void destroyServer();
extern void serverBefore();
extern void serverAfter();

// Client
extern int createClient(char* ipAddress, int port);
extern void destroyClient();
extern void clientBefore();
extern void clientAfter();


void initEnet();
void cleanupEnet();

extern Game game;
