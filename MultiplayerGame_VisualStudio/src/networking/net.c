#include "net.h"

static ENetAddress address;
static int hostType = NET_HOST_NONE;

static int (*netCreate)(void);
static void (*netDispose)(void);
static void (*netReceive)(void);
static void (*netSend)(void);

void initEnet()
{
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
}

void cleanupEnet()
{
    if (netDispose != NULL)
        netDispose();

    enet_deinitialize();
}

void setHostType(int type)
{
    // Set callbacks for server
    if (type == NET_HOST_SERVER)
    {
        hostType = NET_HOST_SERVER;
        netCreate = createServer;
        netDispose = destroyServer;
        netReceive = serverBefore;
        netSend = serverAfter;
    }

    // Set callbacks for client
    if (type == NET_HOST_CLIENT)
    {
        hostType = NET_HOST_CLIENT;
        netCreate = createClient;
        netDispose = destroyClient;
        netReceive = clientBefore;
        netSend = clientAfter;
    }
}

int getHostType()
{
    return hostType;
}

char* hostTypeToString(int type)
{
    switch (type)
    {
    case NET_HOST_NONE:
        return "None";
    case NET_HOST_SERVER:
        return "Server";
    case NET_HOST_CLIENT:
        return "Client";
    }
}

void setConnectionString(char* ipAddress, int port)
{
    enet_address_set_host_ip(&address, ipAddress);
    address.port = port;
}
char* getConnectionString()
{
    char buffer[22];
    //sprintf_s(buffer, sizeof(buffer), "%s:%d", enet_address_get_host_ip(&address, ), address.port);
    return buffer;
}

int createHost()
{
    if (netCreate != NULL)
        return netCreate();

    return EXIT_FAILURE;
}

void disposeHost()
{
    if (netDispose != NULL)
        netDispose();
    hostType = NET_HOST_NONE;
}

void doNetworkingBefore()
{
    if (netReceive != NULL)
        netReceive();
}

void doNetworkingAfter()
{
    if (netSend != NULL)
        netSend();
}
