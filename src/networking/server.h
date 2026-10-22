#pragma once

#include "../core/common.h"

#include "enet/enet.h"


void setServerOnConnect(void (*onConnectOk)(void));
void setServerOnDisconnect(void (*onDisconnect)(void));
void setServerHandlers(const ServerHandlers* serverHandlers);
int createServer(char* ipAddress, int port);
void destroyServer(void);
void serverSend(int client, const void* data, size_t size);
void serverBroadcast(const void* data, size_t size);
int serverClientsCount(void);
void serverBefore(void);
void serverAfter(void);


// Utils
extern int isValidIPAddress(const char* ipAddress);
extern void secure_strcpy(char* dest, size_t destSize, const char* src);
