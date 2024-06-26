#include "client.h"


static void handleConnection(void);
static void handleMessage(ENetPacket* packet);
static void (*connectOkCallback)(void);
static void (*connectFailCallback)(void);
static void (*disconnectCallback)(void);


static ENetAddress serverAddress;
static ENetHost* clientHost = NULL;
static ENetPeer* serverPeer = NULL;
static int isWaiting = 0;
static Uint32 startWaitingTime = 0;


void setClientOnConnect(void (*onConnectOk)(void), void (*onConnectFail)(void))
{
    connectOkCallback = onConnectOk;
    connectFailCallback = onConnectFail;
}

void setClientOnDisconnect(void (*onDisconnect)(void))
{
    disconnectCallback = onDisconnect;
}

int createClient(char* ipAddress, int port)
{
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
        return -1;
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

        return -1;
    }

    // Connection attempt
    isWaiting = 1;
    startWaitingTime = SDL_GetTicks();

    return 0;
}

void destroyClient(void)
{
    if (clientHost == NULL)
        return;

    if (serverPeer != NULL)
        enet_peer_disconnect_now(serverPeer, 0);

    enet_host_destroy(clientHost);
    clientHost = NULL;

    if (disconnectCallback != NULL)
        disconnectCallback();

    isWaiting = 0;
    startWaitingTime = 0;
}


static void handleConnection(void)
{
    printf("Received connect event\n");
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

            if (connectOkCallback != NULL)
                connectOkCallback();

            // Test
            printf("CONNECT_OK: %d\n", connectOK.localID);

            isWaiting = 0;
            startWaitingTime = 0;

            break;
        }
        case CONNECT_DENIED:
        {
            ConnectDenied connectDenied = receivedNetMessage.data.connectDenied;

            // TODO
            printf("Connect Denied message: %s\n", connectDenied.message);

            destroyClient();

            if (connectFailCallback != NULL)
                connectFailCallback();

            isWaiting = 0;
            startWaitingTime = 0;

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
        case PLAYER_LEFT:
        {
            PlayerLeft playerLeft= receivedNetMessage.data.playerLeft;

            if (getPlayerByID(playerLeft.oldPlayer.id) != NULL)
            {
                destroyPlayerByID(playerLeft.oldPlayer.id);
            }

            // Test
            printf("PLAYER_LEFT: %d\n", playerLeft.oldPlayer.id);

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

void clientBefore(void)
{
    ENetEvent event;

    if (clientHost == NULL)
        return;

    if (isWaiting == 1 && SDL_GetTicks() - startWaitingTime >= 3000)
    {
        printf("Connection attempt failed.\n");
        connectFailCallback();
        destroyClient();
        return;
    }

    while (enet_host_service(clientHost, &event, 0) > 0)
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

                destroyClient();

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

// sends player update
void clientAfter(void)
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

