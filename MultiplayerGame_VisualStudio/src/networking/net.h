#pragma once

#include "../common.h"

#include "enet/enet.h"


void initEnet(void);
void cleanupEnet(void);
void setHostType(int type);
void setAfterConnect(void (*callbackOk)(void), void (*callbackFail)(void));
void setAfterDisconnect(void (*callback)(void));
int getHostType(void);
char* hostTypeToString(int type);
void setConnectionString(char* ipAddress, int port);
char* getConnectionString(void);
int netCreateHost(char* ipAddress, int port);
void netDisposeHost(void);
void doNetworkingBefore(void);
void doNetworkingAfter(void);


// Server
extern void setServerOnConnect(void (*onConnect)(void));
extern void setServerOnDisconnect(void (*onDisconnect)(void));
extern int createServer(char* ipAddress, int port);
extern void destroyServer(void);
extern void serverBefore(void);
extern void serverAfter(void);

// Client
extern void setClientOnConnect(void (*onConnectOk)(void), void (*onConnectFail)(void));
extern void setClientOnDisconnect(void (*onDisconnect)(void));
extern int createClient(char* ipAddress, int port);
extern void destroyClient(void);
extern void clientBefore(void);
extern void clientAfter(void);


extern Game game;
