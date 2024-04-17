#include "server.h"

typedef struct {
    ENetPeer* peer;
    int id;
    float x, y;
} Player;

Player players[4];
int numPlayers = 1;

ENetAddress serverAddress;
ENetHost* serverHost = NULL;

ENetPeer* clientPeers[MAX_PLAYERS];
int numPeers = 0;

int createServer()
{
    // Bind the server to the default localhost.
    // A specific host address can be specified by
    // enet_address_set_host (& address, "x.x.x.x");
    serverAddress.host = ENET_HOST_ANY;
    serverAddress.port = PORT;

    serverHost = enet_host_create(
        &serverAddress, // the address to bind the server host to 
        MAX_PLAYERS,    // allow up to 4 clients and/or outgoing connections
        2,              // allow up to 2 channels to be used, 0 and 1
        0,              // assume any amount of incoming bandwidth
        0               // assume any amount of outgoing bandwidth
    );

    if (serverHost == NULL)
    {
        printf("An error occurred while trying to create an ENet server host.\n");
        return EXIT_FAILURE;
    }

    // Create player
    int id = spawnPlayerAt(50.0f, 50.0f);
    setLocalPlayer(getPlayerByID(id));

    return EXIT_SUCCESS;
}

void destroyServer()
{
    ENetEvent event;

    enet_host_destroy(serverHost);

    for (int i = 0; i < numPeers; i++)
    {
        if (clientPeers[i] != NULL)
        {
            enet_peer_disconnect(clientPeers[i], 0);
            
            while (enet_host_service(serverHost, &event, 500) > 0)
            {
                if (event.type == ENET_EVENT_TYPE_DISCONNECT)
                    break;
            }

            enet_peer_reset(clientPeers[i]);
        }
    }
}

static void handleConnection(ENetPeer* peer)
{
    // Check if the server is full
    printf("TEST: handleConnection() - 1\n");
    if (numPeers == MAX_PLAYERS)
    {
        // Send back CONNECT_DENIED
        NetMessage message;
        memset(&message, 0, sizeof(NetMessage));

        message.type = CONNECT_DENIED;
        strcpy_s(message.data.connectDenied.message, 64, "The server is full.");

        ENetPacket* packet = enet_packet_create(&message, sizeof(NetMessage), ENET_PACKET_FLAG_RELIABLE);
        
        return;
    }

    // The player can be accepted
    clientPeers[numPeers] = peer;
    numPeers++;

    // Spawn new player entity
    int id = spawnPlayerAt(50.0f, 50.0f);

    // Save the player's ID inside the peer's data field
    peer->data = (int)id;

    // Add the client to peers array
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (clientPeers[i] == NULL)
        {
            clientPeers[i] = peer;
            break;
        }
    }

    // Send back player's info
    NetMessage message;
    memset(&message, 0, sizeof(NetMessage));

    message.type = CONNECT_OK;
    message.data.connectOK.localID = id;

    Entity* player;
    for (player = game.playersHead.next; player != NULL; player = player->next)
    {
        message.data.connectOK.players[message.data.connectOK.numPlayers] = *player;
        message.data.connectOK.numPlayers++;
    }

    ENetPacket* packet = enet_packet_create(&message, sizeof(NetMessage), ENET_PACKET_FLAG_RELIABLE);

    // Broadcast the game state to each connected peer
    enet_host_broadcast(serverHost, 0, packet);

    // Test
    enet_host_flush(serverHost);

    // test
    printf("Peer.connectID: %u\nPeer.incomingPeerID: %u\n", peer->connectID, peer->incomingPeerID);
}

static void handleDisconnection(ENetPeer* peer)
{
    // Destroy player entity
    destroyPlayerByID((int)peer->data);

    // Remove client from peers array
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (clientPeers[i] == peer) {
            clientPeers[i] = NULL;
            numPeers--;
            break;
        }
    }

    // test
    printf("Server.c: destroy %d\n", (int)peer->data);
}

static void handleMessage(ENetPeer* peer, ENetPacket* packet)
{
    PlayerState receivedState;
    memcpy(&receivedState, packet->data, sizeof(PlayerState));

    // Update player's position
    Entity* player = getPlayerByID((int)peer->data);
    player->x = receivedState.player.x;
    player->y = receivedState.player.y;

    // Clean up the packet now that we're done using it
    enet_packet_destroy(packet);
}

void serverBefore()
{
    ENetEvent event;
    ENetPacket* packet;

    if (serverHost == NULL)
    {
        printf("Server is NULL.\n");
        return;
    }

    while (enet_host_service(serverHost, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            printf("A new client connected from %x:%u.\n",
                event.peer->address.host,
                event.peer->address.port);

            handleConnection(event.peer);

            enet_packet_destroy(event.packet);

            break;

        case ENET_EVENT_TYPE_RECEIVE:
            /*printf("A packet of length %u containing %s was received from P%d on channel %u.\n",
                event.packet->dataLength,
                event.packet->data,
                (int)event.peer->data,
                event.channelID);*/

            handleMessage(event.peer, event.packet);

            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            printf("P%d disconnected.\n", (int)event.peer->data);

            handleDisconnection(event.peer);
            numPeers--;

            /*for (int i = 0; i < MAX_PLAYERS; i++) {
                if (clientPeers[i] == event.peer) {
                    clientPeers[i] = NULL;
                    break;
                }
            }*/

            break;
        }
    }
}

void serverAfter()
{
    // Send updated game state
    
    NetMessage message;
    memset(&message, 0, sizeof(NetMessage));
    
    message.type = GAME_STATE;

    Entity* player;
    for (player = game.playersHead.next; player != NULL; player = player->next)
    {
        message.data.gameState.players[message.data.gameState.numPlayers] = *player;
        message.data.gameState.numPlayers++;
    }

    ENetPacket* packet = enet_packet_create(&message, sizeof(NetMessage), ENET_PACKET_FLAG_RELIABLE);
    
    // Broadcast the game state to each connected peer
    enet_host_broadcast(serverHost, 0, packet);

    // Test
    enet_host_flush(serverHost);
}