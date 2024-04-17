#pragma once

#include "../common.h"

#include "enet/enet.h"

// Server
extern int createServer();
extern void destroyServer();
extern void serverBefore();
extern void serverAfter();

// Client
extern int createClient();
extern void destroyClient();
extern void clientBefore();
extern void clientAfter();


void initEnet();
void cleanupEnet();

extern Game game;
