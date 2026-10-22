#include "server.h"


/*
* State of a running server: allocated by createServer and freed by destroyServer,
* so nothing can leak from one session into the next.
*/
typedef struct {
    ENetAddress address;
    ENetHost* host;
    ENetPeer* clientPeers[MAX_SERVER_PEERS]; // Connected clients, indexed by client slot
    int numPeers;
} ServerSession;


static void handleConnection(ENetPeer* peer);
static void handleDisconnection(ENetPeer* peer);
static int findClient(ENetPeer* peer);
static void (*connectCallback)(void);
static void (*disconnectCallback)(void);


static ServerSession* session = NULL;
static ServerHandlers handlers;


void setServerOnConnect(void (*onConnect)(void))
{
    connectCallback = onConnect;
}

void setServerOnDisconnect(void (*onDisconnect)(void))
{
    disconnectCallback = onDisconnect;
}

void setServerHandlers(const ServerHandlers* serverHandlers)
{
    handlers = *serverHandlers;
}

int createServer(char* ipAddress, int port)
{
    if (session != NULL)
    {
        printf("A server session is already running.\n");
        return -1;
    }

    session = calloc(1, sizeof(ServerSession));
    if (session == NULL)
    {
        printf("Couldn't allocate the server session.\n");
        return -1;
    }

    if (ipAddress == NULL || strlen(ipAddress) == 0 || !isValidIPAddress(ipAddress))
    {
        session->address.host = ENET_HOST_ANY;
    }
    else if (enet_address_set_host(&session->address, ipAddress) < 0)
    {
        printf("Couldn't resolve host %s\n", ipAddress);
        free(session);
        session = NULL;
        return -1;
    }
    session->address.port = port;

    session->host = enet_host_create(
        &session->address,  // the address to bind the server host to
        MAX_SERVER_PEERS + 1, // one extra slot to accept, then deny, connections when the server is full
        2,                  // allow up to 2 channels to be used, 0 and 1
        0,                  // assume any amount of incoming bandwidth
        0                   // assume any amount of outgoing bandwidth
    );

    if (session->host == NULL)
    {
        printf("An error occurred while trying to create an ENet server host.\n");
        free(session);
        session = NULL;
        return -1;
    }

    if (handlers.onStart != NULL && handlers.onStart() < 0)
    {
        enet_host_destroy(session->host);
        free(session);
        session = NULL;
        return -1;
    }

    if (connectCallback != NULL)
        connectCallback();

    return 0;
}

void destroyServer(void)
{
    if (session == NULL)
        return;

    for (int i = 0; i < MAX_SERVER_PEERS; i++)
    {
        if (session->clientPeers[i] != NULL)
        {
            printf("Sent disconnect to client %d\n", i);
            enet_peer_disconnect_now(session->clientPeers[i], 0);
        }
    }

    enet_host_destroy(session->host);
    free(session);
    session = NULL;

    if (disconnectCallback != NULL)
        disconnectCallback();
}

void serverSend(int client, const void* data, size_t size)
{
    ENetPacket* packet;

    if (session == NULL || client < 0 || client >= MAX_SERVER_PEERS || session->clientPeers[client] == NULL)
        return;

    packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(session->clientPeers[client], 0, packet);
}

void serverBroadcast(const void* data, size_t size)
{
    ENetPacket* packet;

    if (session == NULL)
        return;

    packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(session->host, 0, packet);
}

int serverClientsCount(void)
{
    return session != NULL ? session->numPeers : 0;
}

static int findClient(ENetPeer* peer)
{
    for (int i = 0; i < MAX_SERVER_PEERS; i++)
    {
        if (session->clientPeers[i] == peer)
            return i;
    }

    return -1;
}

static void handleConnection(ENetPeer* peer)
{
    NetMessage message;
    ENetPacket* packet;
    int client;

    // Send CONNECT_DENIED if the server is full.
    if (session->numPeers >= MAX_SERVER_PEERS)
    {
        memset(&message, 0, sizeof(NetMessage));
        message.type = CONNECT_DENIED;
        secure_strcpy(message.data.connectDenied.message, 64, "The server is full.");

        packet = enet_packet_create(&message, sizeof(NetMessage), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);

        // Disconnect once the queued CONNECT_DENIED has been sent
        enet_peer_disconnect_later(peer, 0);

        return;
    }

    // The client can be accepted, so we add it in the first available slot
    for (client = 0; client < MAX_SERVER_PEERS; client++)
    {
        if (session->clientPeers[client] == NULL)
        {
            session->clientPeers[client] = peer;
            session->numPeers++;
            break;
        }
    }

    if (handlers.onClientJoin != NULL && handlers.onClientJoin(client) < 0)
    {
        // Refused by the game: free the slot, the later disconnect event is ignored for peers not in the array
        session->clientPeers[client] = NULL;
        session->numPeers--;
        enet_peer_disconnect(peer, 0);
    }
}

static void handleDisconnection(ENetPeer* peer)
{
    // No enet_peer_reset: ENet already resets the peer before returning the disconnect event
    int client = findClient(peer);

    // Denied peers were never added to the array
    if (client < 0)
        return;

    session->clientPeers[client] = NULL;
    session->numPeers--;

    if (handlers.onClientLeave != NULL)
        handlers.onClientLeave(client);
}

void serverBefore(void)
{
    ENetEvent event;
    int client;

    if (session == NULL)
        return;

    while (enet_host_service(session->host, &event, 0) > 0)
    {
        switch (event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                handleConnection(event.peer);

                printf("A new client connected from %x:%u. Num Peers: %d\n",
                    event.peer->address.host,
                    event.peer->address.port,
                    session->numPeers);

                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                handleDisconnection(event.peer);

                printf("A client disconnected (%x:%u). Num Peers: %d\n",
                    event.peer->address.host,
                    event.peer->address.port,
                    session->numPeers);

                break;
            }
            case ENET_EVENT_TYPE_RECEIVE:
            {
                client = findClient(event.peer);
                if (client >= 0 && handlers.onClientData != NULL)
                    handlers.onClientData(client, event.packet->data, event.packet->dataLength);

                enet_packet_destroy(event.packet);

                break;
            }
            default:
            {
                printf("Unhandled event type\n");

                break;
            }
        }
    }
}

/*
* Runs after the game loop: sends the queued packets.
*/
void serverAfter(void)
{
    if (session == NULL)
        return;

    enet_host_flush(session->host);
}

