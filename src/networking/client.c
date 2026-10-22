#include "client.h"
#include "protocol.h"


/*
* State of a client session: allocated by createClient and freed by destroyClient,
* so nothing can leak from one session into the next.
*/
typedef struct {
    ENetAddress serverAddress;
    ENetHost* host;
    ENetPeer* serverPeer;
    int isWaiting;              // Connection attempt in progress (CONNECT_OK not received yet)
    Uint32 startWaitingTime;
} ClientSession;


static void handleConnection(void);
static void handleMessage(ENetPacket* packet);
static void (*connectOkCallback)(void);
static void (*connectFailCallback)(void);
static void (*disconnectCallback)(void);


static ClientSession* session = NULL;
static ClientHandlers handlers;


void setClientOnConnect(void (*onConnectOk)(void), void (*onConnectFail)(void))
{
    connectOkCallback = onConnectOk;
    connectFailCallback = onConnectFail;
}

void setClientOnDisconnect(void (*onDisconnect)(void))
{
    disconnectCallback = onDisconnect;
}

void setClientHandlers(const ClientHandlers* clientHandlers)
{
    handlers = *clientHandlers;
}

static void freeSession(void)
{
    if (session->host != NULL)
        enet_host_destroy(session->host);

    free(session);
    session = NULL;
}

int createClient(char* ipAddress, int port)
{
    if (session != NULL)
    {
        printf("A client session is already running.\n");
        return -1;
    }

    session = calloc(1, sizeof(ClientSession));
    if (session == NULL)
    {
        printf("Couldn't allocate the client session.\n");
        return -1;
    }

    // Create client host
    session->host = enet_host_create(
        NULL,   // create a client host
        1,      // only allow 1 outgoing connection
        2,      // allow up 2 channels to be used, 0 and 1
        0,      // assume any amount of incoming bandwidth
        0       // assume any amount of outgoing bandwidth
    );

    if (session->host == NULL)
    {
        printf("An error occurred while trying to create an ENet client host.\n");
        freeSession();
        return -1;
    }

    const char* host = HOSTNAME;

    if (ipAddress != NULL && strlen(ipAddress) > 0)
    {
        if (!isValidIPAddress(ipAddress))
        {
            printf("Invalid IP address\n");
            freeSession();
            return -1;
        }
        host = ipAddress;
    }

    printf("Attempting to connect to %s:%d...\n", host, port);
    if (enet_address_set_host(&session->serverAddress, host) < 0)
    {
        printf("Couldn't resolve host %s\n", host);
        freeSession();
        return -1;
    }
    session->serverAddress.port = port;

    // Connect to server
    session->serverPeer = enet_host_connect(session->host, &session->serverAddress, 2, 0);
    if (session->serverPeer == NULL) {
        printf("No available peers for initiating an ENet connection\n");
        freeSession();
        return -1;
    }

    // Connection attempt
    session->isWaiting = 1;
    session->startWaitingTime = SDL_GetTicks();

    return 0;
}

void destroyClient(void)
{
    int wasConnected;

    if (session == NULL)
        return;

    enet_peer_disconnect_now(session->serverPeer, 0);

    // Only established connections report a disconnection: failed attempts use connectFailCallback
    wasConnected = !session->isWaiting;

    freeSession();

    if (wasConnected && disconnectCallback != NULL)
        disconnectCallback();
}


static void handleConnection(void)
{
    printf("Received connect event\n");
}

static void handleMessage(ENetPacket* packet)
{
    NetMessage receivedNetMessage;

    if (readMessage(packet->data, packet->dataLength, &receivedNetMessage) < 0)
    {
        printf("Invalid message from the server (%zu bytes)\n", packet->dataLength);
        return;
    }

    // The server refused the connection: part of the handshake, handled here
    if (receivedNetMessage.type == CONNECT_DENIED)
    {
        switch (receivedNetMessage.data.connectDenied.reason)
        {
            case DENY_SERVER_FULL:
                printf("Connection denied: the server is full.\n");
                break;
            case DENY_VERSION_MISMATCH:
                printf("Connection denied: the server runs a different version.\n");
                break;
        }

        destroyClient();

        if (connectFailCallback != NULL)
            connectFailCallback();

        return;
    }

    // Any other message is up to the game
    if (handlers.onMessage == NULL || handlers.onMessage(&receivedNetMessage) < 0)
        return;

    // A valid CONNECT_OK completes the connection attempt
    if (receivedNetMessage.type == CONNECT_OK && session->isWaiting)
    {
        session->isWaiting = 0;

        if (connectOkCallback != NULL)
            connectOkCallback();
    }
}

void clientBefore(void)
{
    ENetEvent event;

    if (session == NULL)
        return;

    // Long enough to receive CONNECT_DENIED while ENet retries a connect to a full server
    if (session->isWaiting && SDL_GetTicks() - session->startWaitingTime >= 5000)
    {
        printf("Connection attempt failed.\n");
        destroyClient();

        if (connectFailCallback != NULL)
            connectFailCallback();

        return;
    }

    // A handled message may destroy the session (e.g. CONNECT_DENIED)
    while (session != NULL && enet_host_service(session->host, &event, 0) > 0)
    {
        switch (event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                printf("EVENT_CONNECT\n");

                handleConnection();

                return;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                printf("EVENT_DISCONNECT\n");

                // Disconnected before CONNECT_OK: the connection attempt failed
                int wasWaiting = session->isWaiting;

                destroyClient();

                if (wasWaiting && connectFailCallback != NULL)
                    connectFailCallback();

                return;
            }
            case ENET_EVENT_TYPE_RECEIVE:
            {
                handleMessage(event.packet);

                enet_packet_destroy(event.packet);

                break;
            }
            default:
            {
                printf("EVENT_UNKNOWN\n");

                break;
            }
        }
    }
}

void clientSend(const NetMessage* message)
{
    Uint8 buffer[NET_MAX_MESSAGE_SIZE];
    ENetPacket* packet;
    size_t size;

    if (session == NULL)
        return;

    size = writeMessage(message, buffer, sizeof(buffer));
    if (size == 0)
    {
        printf("Couldn't serialize message of type %d\n", message->type);
        return;
    }

    packet = enet_packet_create(buffer, size, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(session->serverPeer, 0, packet);
}

/*
* Runs after the game loop: sends the queued packets.
*/
void clientAfter(void)
{
    if (session == NULL)
        return;

    enet_host_flush(session->host);
}
