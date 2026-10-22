#pragma once

#include "../core/common.h"

#include "enet/enet.h"


void setServerOnConnect(void (*onConnectOk)(void));
void setServerOnDisconnect(void (*onDisconnect)(void));
void setServerHandlers(const ServerHandlers* serverHandlers);
int createServer(char* ipAddress, int port);
void destroyServer(void);
void serverSend(int client, const NetMessage* message);
void serverBroadcast(const NetMessage* message);
int serverClientsCount(void);
void serverBefore(void);
void serverAfter(void);


// Utils
extern int isValidIPAddress(const char* ipAddress);
