#include "server.h"

void setServerOnConnect(void (*onConnectOk)(void));
void setServerOnDisconnect(void (*onDisconnect)(void));
int createServer(char* ipAddress, int port);
void destroyServer();
void serverBefore();
void serverAfter();
static void handleConnection(ENetPeer* peer);
static void handleDisconnection(ENetPeer* peer);
static void handleMessage(ENetPeer* peer, ENetPacket* packet);
static void (*connectCallback)(void);
static void (*disconnectCallback)(void);


static ENetAddress serverAddress;
static ENetHost* serverHost = NULL;
static ENetPeer* clientPeers[MAX_SERVER_PEERS]; // Connected clients
static int numPeers = 0;


void setServerOnConnect(void (*onConnect)(void))
{
    connectCallback = onConnect;
}

void setServerOnDisconnect(void (*onDisconnect)(void))
{
    disconnectCallback = onDisconnect;
}

int createServer(char* ipAddress, int port)
{
    int id;

    if (ipAddress == NULL || strlen(ipAddress) == 0 || !isValidIPAddress(ipAddress))
    {
        serverAddress.host = ENET_HOST_ANY;
    }
    else
    {
        enet_address_set_host(&serverAddress, ipAddress);
    }
    serverAddress.port = port;

    serverHost = enet_host_create(
        &serverAddress,     // the address to bind the server host to 
        MAX_SERVER_PEERS,   // allow up to 4 clients and/or outgoing connections
        2,                  // allow up to 2 channels to be used, 0 and 1
        0,                  // assume any amount of incoming bandwidth
        0                   // assume any amount of outgoing bandwidth
    );

    if (serverHost == NULL)
    {
        printf("An error occurred while trying to create an ENet server host.\n");
        return -1;
    }

    // Create player
    id = spawnPlayerAt(50.0f, 50.0f);
    setLocalPlayer(getPlayerByID(id));

    connectCallback();

    return 0;
}

void destroyServer()
{
    ENetEvent event;

    if (serverHost == NULL)
        return;

    for (int i = 0; i < MAX_SERVER_PEERS; i++)
    {
        if (clientPeers[i] != NULL)
        {
            printf("Sent disconnect to P%d\n", (int) clientPeers[i]->data);
            enet_peer_disconnect_now(clientPeers[i], 0);

            // CHECK
            /*while (enet_host_service(serverHost, &event, 0) > 0)
            {
                if (event.type == ENET_EVENT_TYPE_DISCONNECT)
                    continue;
            }

            enet_peer_reset(clientPeers[i]);*/
        }
    }

    enet_host_destroy(serverHost);
    serverHost = NULL;

    if (disconnectCallback != NULL)
        disconnectCallback();
}

static void handleConnection(ENetPeer* peer)
{
    NetMessage message;
    Entity* player;
    ENetPacket* packet;
    int iPeer, id;

    memset(&message, 0, sizeof(NetMessage));

    // Send CONNECT_DENIED if the server is full.
    if (numPeers >= MAX_SERVER_PEERS)
    {
        message.type = CONNECT_DENIED;
        secure_strcpy(message.data.connectDenied.message, 64, "The server is full.");

        packet = enet_packet_create(&message, sizeof(NetMessage), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);

        enet_peer_reset(peer);

        return;
    }

    // The player can be accepted, so we add it in the first available spot
    for (iPeer = 0; iPeer < MAX_SERVER_PEERS; iPeer++)
    {
        if (clientPeers[iPeer] == NULL)
        {
            clientPeers[iPeer] = peer;
            numPeers++;
            break;
        }
    }

    // Spawn new player entity
    id = spawnPlayerAt(50.0f, 50.0f);

    // Save the player's ID inside the peer's data field
    clientPeers[iPeer]->data = id;

    // Send CONNECT_OK to the new player
    message.type = CONNECT_OK;
    message.data.connectOK.localID = id;

    for (player = game.playersHead.next; player != NULL; player = player->next)
    {
        message.data.connectOK.players[message.data.connectOK.numPlayers] = *player;
        message.data.connectOK.numPlayers++;
    }

    packet = enet_packet_create(&message, sizeof(NetMessage), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);

    // Broadcast PLAYER_JOINED to each player
    memset(&message, 0, sizeof(NetMessage));
    message.type = PLAYER_JOINED;
    message.data.playerJoined.newPlayer = *(getPlayerByID(id));

    packet = enet_packet_create(&message, sizeof(NetMessage), ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(serverHost, 0, packet);
}

static void handleDisconnection(ENetPeer* peer)
{
    NetMessage message;
    Entity* player;
    ENetPacket* packet;
    int id;

    // Remove client from peers array
    for (int i = 0; i < MAX_SERVER_PEERS; i++)
    {
        if ((int)clientPeers[i]->data == (int) peer->data)
        {
            clientPeers[i] = NULL;
            numPeers--;
            break;
        }
    }
    enet_peer_reset(peer);

    id = (int)peer->data;

    // Broadcast PLAYER_LEFT to each player
    memset(&message, 0, sizeof(NetMessage));
    message.type = PLAYER_LEFT;
    message.data.playerLeft.oldPlayer = *(getPlayerByID(id));

    packet = enet_packet_create(&message, sizeof(NetMessage), ENET_PACKET_FLAG_RELIABLE);
    enet_host_broadcast(serverHost, 0, packet);

    // Destroy player entity
    destroyPlayerByID(id);
}

static void handleMessage(ENetPeer* peer, ENetPacket* packet)
{
    PlayerState receivedState;
    Entity* player;

    memcpy(&receivedState, packet->data, sizeof(PlayerState));

    // Update player's position
    player = getPlayerByID((int)peer->data);
    player->x = receivedState.player.x;
    player->y = receivedState.player.y;
}

void serverBefore()
{
    ENetEvent event;
    ENetPacket* packet = { 0 };

    if (serverHost == NULL)
        return;

    while (enet_host_service(serverHost, &event, 0) > 0)
    {
        switch (event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                handleConnection(event.peer);

                printf("A new client connected from %x:%u. Num Peers: %d\n",
                    event.peer->address.host,
                    event.peer->address.port,
                    numPeers);

                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                handleDisconnection(event.peer);

                printf("P%d disconnected (%x:%u). Num Peers: %d\n",
                    (int)event.peer->data,
                    event.peer->address.host,
                    event.peer->address.port,
                    numPeers);

                break;
            }
            case ENET_EVENT_TYPE_RECEIVE:
            {
                handleMessage(event.peer, event.packet);

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
* Runs after the game loop.
*/
void serverAfter()
{
    NetMessage message;
    Entity* player;
    ENetPacket* packet;

    if (serverHost == NULL || numPeers == 0)
        return;

    memset(&message, 0, sizeof(NetMessage));
    message.type = GAME_STATE;

    // Loops over players list
    for (player = game.playersHead.next; player != NULL; player = player->next)
    {
        message.data.gameState.players[message.data.gameState.numPlayers] = *player;
        message.data.gameState.numPlayers++;
    }

    packet = enet_packet_create(&message, sizeof(NetMessage), ENET_PACKET_FLAG_RELIABLE);
    
    // Broadcast the game state to each connected peer
    enet_host_broadcast(serverHost, 0, packet);
    enet_host_flush(serverHost);
}