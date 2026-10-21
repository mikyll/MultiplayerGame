#include "client.h"


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


void setClientOnConnect(void (*onConnectOk)(void), void (*onConnectFail)(void))
{
    connectOkCallback = onConnectOk;
    connectFailCallback = onConnectFail;
}

void setClientOnDisconnect(void (*onDisconnect)(void))
{
    disconnectCallback = onDisconnect;
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
    int id;

    if (packet->dataLength != sizeof(NetMessage))
    {
        printf("Invalid message size: %zu\n", packet->dataLength);
        return;
    }

    memcpy(&receivedNetMessage, packet->data, sizeof(NetMessage));

    switch (receivedNetMessage.type)
    {
        case CONNECT_OK:
        {
            ConnectOK connectOK = receivedNetMessage.data.connectOK;

            if (connectOK.numPlayers < 0 || connectOK.numPlayers > MAX_PLAYERS)
            {
                printf("Invalid CONNECT_OK players count: %d\n", connectOK.numPlayers);
                break;
            }

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

            session->isWaiting = 0;

            if (connectOkCallback != NULL)
                connectOkCallback();

            // Test
            printf("CONNECT_OK: %d\n", connectOK.localID);

            break;
        }
        case CONNECT_DENIED:
        {
            ConnectDenied connectDenied = receivedNetMessage.data.connectDenied;
            connectDenied.message[sizeof(connectDenied.message) - 1] = '\0';

            // TODO
            printf("Connect Denied message: %s\n", connectDenied.message);

            destroyClient();

            if (connectFailCallback != NULL)
                connectFailCallback();

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

            if (gameState.numPlayers < 0 || gameState.numPlayers > MAX_PLAYERS)
            {
                printf("Invalid GAME_STATE players count: %d\n", gameState.numPlayers);
                break;
            }

            // Update remote players: the local one is driven by input, the server only echoes
            // back an outdated copy of its position
            for (int i = 0; i < gameState.numPlayers; i++)
            {
                Entity* player = getPlayerByID(gameState.players[i].id);
                if (player != NULL && player != getLocalPlayer())
                {
                    player->x = gameState.players[i].x;
                    player->y = gameState.players[i].y;
                    player->color = gameState.players[i].color;
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

// sends player update
void clientAfter(void)
{
    Entity* player;
    PlayerState playerState;
    ENetPacket* packet;

    if (session == NULL)
        return;

    player = getLocalPlayer();
    if (player == NULL)
        return;

    memset(&playerState, 0, sizeof(PlayerState));
    playerState.player = *player;

    // Send local player position
    packet = enet_packet_create(&playerState, sizeof(PlayerState), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(session->serverPeer, 0, packet);
    enet_host_flush(session->host);
}

