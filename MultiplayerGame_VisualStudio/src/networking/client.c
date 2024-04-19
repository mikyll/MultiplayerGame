#include "client.h"

int createClient(char* ipAddress, int port);
void destroyClient();
static void handleMessage(ENetPacket* packet);
void clientBefore();
void clientAfter();


static ENetAddress clientAddress;
static ENetAddress serverAddress;
static ENetHost* clientHost = NULL;
static ENetPeer* serverPeer = NULL;
static int isWaiting = 0;

int createClient(char* ipAddress, int port)
{
    ENetEvent event;

    // Create client host
    clientHost = enet_host_create(
        NULL,   // create a client host
        1,      // only allow 1 outgoing connection
        2,      // allow up 2 channels to be used, 0 and 1
        0,      // assume any amount of incoming bandwidth
        0       // assume any amount of outgoing bandwidth
    );

    if (clientHost == NULL)
    {
        printf("An error occurred while trying to create an ENet client host.\n");
        return EXIT_FAILURE;
    }

    if (ipAddress == NULL || strlen(ipAddress) == 0)
    {
        enet_address_set_host(&serverAddress, HOSTNAME);
    }
    else
    {
        if (isValidIPAddress(ipAddress))
        {
            printf("Attempting to connect to IP address %s:%d...\n", ipAddress, port);
            enet_address_set_host(&serverAddress, ipAddress);
        }
        else
        {
            printf("Invalid IP address\n");
            return -1;
        }
    }
    serverAddress.port = port;

    // Connect to server
    serverPeer = enet_host_connect(clientHost, &serverAddress, 2, 0);
    if (serverPeer == NULL) {
        printf("No available peers for initiating an ENet connection\n");

        return EXIT_FAILURE;
    }

    // Wait for connection response
    // TODO fix (it's blocking)
    while (enet_host_service(clientHost, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        printf("Connection to %s:%d succeeded.\n", ipAddress, port);

        return EXIT_SUCCESS;
    }

    enet_peer_reset(serverPeer);
    fprintf(stderr, "Connection to %s:%d failed.\n", ipAddress, port);

    return EXIT_FAILURE;
}

void destroyClient()
{
    if (serverPeer != NULL)
        enet_peer_disconnect_now(serverPeer, 0);

    if (clientHost != NULL)
        enet_host_destroy(clientHost);
}

static void handleMessage(ENetPacket* packet)
{
    NetMessage receivedNetMessage;
    int id;
    
    memcpy(&receivedNetMessage, packet->data, sizeof(NetMessage));

    switch (receivedNetMessage.type)
    {
        case CONNECT_OK:
        {
            ConnectOK connectOK = receivedNetMessage.data.connectOK;

            // Spawn missing players
            for (int i = 0; i < connectOK.numPlayers; i++)
            {
                if (getPlayerByID(connectOK.players[i].id) == NULL)
                {
                    id = spawnPlayer(connectOK.players[i]);

                    // Set local player
                    if (connectOK.localID == connectOK.players[i].id)
                    {
                        setLocalPlayer(getPlayerByID(id));
                    }
                }
            }

            // Test
            printf("CONNECT_OK: %d\n", connectOK.localID);

            break;
        }
        case CONNECT_DENIED:
        {
            ConnectDenied connectDenied = receivedNetMessage.data.connectDenied;

            // TODO
            printf("Connect Denied message: %s\n", connectDenied.message);

            destroyClient();

            break;
        }
        case PLAYER_JOINED:
        {
            PlayerJoined playerJoined = receivedNetMessage.data.playerJoined;

            if (getPlayerByID(playerJoined.newPlayer.id) == NULL)
            {
                spawnPlayer(playerJoined.newPlayer);
            }

            // Test
            printf("PLAYER_JOINED: %d\n", playerJoined.newPlayer.id);

            break;
        }
        case GAME_STATE:
        {
            GameState gameState = receivedNetMessage.data.gameState;

            // Update players
            for (int i = 0; i < gameState.numPlayers; i++)
            {
                Entity* player = getPlayerByID(gameState.players[i].id);
                if (player != NULL)
                {
                    player->x = gameState.players[i].x;
                    player->y = gameState.players[i].y;
                }
            }

            break;
        }
        default:
        {
            printf("Unknown message: %d\n", receivedNetMessage.type);
            break;
        }
    }
}

void clientBefore()
{
    ENetEvent event;
    ENetPacket* packet = { 0 };

    if (clientHost == NULL)
        return;

    while (enet_host_service(clientHost, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
        {
            handleMessage(event.packet);

            enet_packet_destroy(event.packet);

            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT:
        {
            destroyClient();

            return;
        }
        default:
        {
            printf("Unhandled event type\n");

            break;
        }
        }
    }
}

// sends player update
void clientAfter()
{
    Entity* player;
    PlayerState playerState;
    ENetPacket* packet;

    if (clientHost == NULL)
        return;

    player = getLocalPlayer();
    if (player == NULL)
        return;

    memset(&playerState, 0, sizeof(PlayerState));
    playerState.player = *player;

    // Send local player position
    packet = enet_packet_create(&playerState, sizeof(PlayerState), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(serverPeer, 0, packet);
    enet_host_flush(clientHost);
}