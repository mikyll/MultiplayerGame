#include "net.h"

static ENetAddress address;
static int hostType = NET_HOST_NONE;

static int (*netCreate)(char* ipAddress, int port);
static void (*netDispose)(void);
static void (*netReceive)(void);
static void (*netSend)(void);

void initEnet(void)
{
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        return;
    }
}

void cleanupEnet(void)
{
    if (netDispose != NULL)
        netDispose();

    enet_deinitialize();
}

void setHostType(int type)
{
    // Set host callbacks
    switch (type)
    {
        case NET_HOST_SERVER:
        {
            netCreate = createServer;
            netDispose = destroyServer;
            netReceive = serverBefore;
            netSend = serverAfter;

            break;
        }
        case NET_HOST_CLIENT:
        {
            netCreate = createClient;
            netDispose = destroyClient;
            netReceive = clientBefore;
            netSend = clientAfter;

            break;
        }
        case NET_HOST_NONE:
        {
            netCreate = NULL;
            netDispose = NULL;
            netReceive = NULL;
            netSend = NULL;

            break;
        }
        default:
        {
            printf("Unknown host type: %d\n", type);
            return;
        }
    }

    hostType = type;
}

void setAfterConnect(void (*callbackOk)(void), void (*callbackFail)(void))
{
    switch (hostType)
    {
        case NET_HOST_SERVER:
        {
            setServerOnConnect(callbackOk);
            break;
        }
        case NET_HOST_CLIENT:
        {
            setClientOnConnect(callbackOk, callbackFail);
            break;
        }
    }
}

void setAfterDisconnect(void (*callback)(void))
{
    switch (hostType)
    {
        case NET_HOST_SERVER:
        {
            setServerOnDisconnect(callback);
            break;
        }
        case NET_HOST_CLIENT:
        {
            setClientOnDisconnect(callback);
            break;
        }
    }
}

int getHostType(void)
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
    default:
        return "Unknown";
    }
}

void setConnectionString(char* ipAddress, int port)
{
    // TODO
    enet_address_set_host_ip(&address, ipAddress);
    address.port = port;
}

char* getConnectionString(void)
{
    // TODO
    /*char buffer[22] = { 0 };
    secure_sprintf(buffer, sizeof(buffer), "%s:%d", enet_address_get_host_ip(&address, ), address.port);*/
    return "";
}

int netCreateHost(char* ipAddress, int port)
{
    if (netCreate != NULL)
        return netCreate(ipAddress, port);

    return EXIT_FAILURE;
}

void netDisposeHost(void)
{
    if (netDispose != NULL)
        netDispose();

    hostType = NET_HOST_NONE;
}

void doNetworkingBefore(void)
{
    if (netReceive != NULL)
        netReceive();
}

void doNetworkingAfter(void)
{
    if (netSend != NULL)
        netSend();
}
