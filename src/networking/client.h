#pragma once

#include "../core/common.h"

#include "enet/enet.h"


void setClientOnConnect(void (*onConnectOk)(void), void (*onConnectFail)(void));
void setClientOnDisconnect(void (*onDisconnect)(void));
void setClientHandlers(const ClientHandlers* clientHandlers);
int createClient(char* ipAddress, int port);
void destroyClient(void);
void clientBefore(void);
void clientSend(const void* data, size_t size);
void clientAfter(void);


// Utils
extern int isValidIPAddress(const char* ipAddress);
