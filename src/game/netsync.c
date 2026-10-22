#include "netsync.h"

/*
* Game side of the network protocol: turns connection events and received messages
* into game state changes, and game state into messages. The networking layer only
* transports the data.
*/


static int onServerStart(void);
static int onClientJoin(int client);
static void onClientLeave(int client);
static void onClientData(int client, const void* data, size_t size);
static void sendServerState(void);


static int clientPlayers[MAX_SERVER_PEERS]; // Player ID of each connected client, by client slot


void initNetworkSync(void)
{
    ServerHandlers serverHandlers = { onServerStart, onClientJoin, onClientLeave, onClientData };

    for (int i = 0; i < MAX_SERVER_PEERS; i++)
        clientPlayers[i] = -1;

    setServerHandlers(&serverHandlers);
}

/*
* Runs after the game logic: sends this instance's view of the game.
*/
void sendGameUpdate(void)
{
    if (getHostType() == NET_HOST_SERVER)
        sendServerState();
}


// Server

static int onServerStart(void)
{
    // The server is also a player
    int id = spawnPlayerAt(50.0f, 50.0f);
    if (id < 0)
        return -1;

    setLocalPlayer(getPlayerByID(id));

    return 0;
}

static int onClientJoin(int client)
{
    NetMessage message;
    Entity* player;
    int id;

    // Spawn new player entity
    id = spawnPlayerAt(50.0f, 50.0f);
    if (id < 0)
        return -1;

    clientPlayers[client] = id;

    // Send CONNECT_OK to the new player
    memset(&message, 0, sizeof(NetMessage));
    message.type = CONNECT_OK;
    message.data.connectOK.localID = id;

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        player = &game.players[i];
        if (!player->active)
            continue;

        message.data.connectOK.players[message.data.connectOK.numPlayers] = *player;
        message.data.connectOK.numPlayers++;
    }

    serverSend(client, &message, sizeof(NetMessage));

    // Broadcast PLAYER_JOINED to each player
    memset(&message, 0, sizeof(NetMessage));
    message.type = PLAYER_JOINED;
    message.data.playerJoined.newPlayer = *(getPlayerByID(id));

    serverBroadcast(&message, sizeof(NetMessage));

    return 0;
}

static void onClientLeave(int client)
{
    NetMessage message;
    Entity* player;
    int id = clientPlayers[client];

    clientPlayers[client] = -1;

    player = getPlayerByID(id);
    if (player == NULL)
        return;

    // Broadcast PLAYER_LEFT to each player
    memset(&message, 0, sizeof(NetMessage));
    message.type = PLAYER_LEFT;
    message.data.playerLeft.oldPlayer = *player;

    serverBroadcast(&message, sizeof(NetMessage));

    // Destroy player entity
    destroyPlayerByID(id);
}

static void onClientData(int client, const void* data, size_t size)
{
    PlayerState receivedState;
    Entity* player;

    if (size != sizeof(PlayerState))
    {
        printf("Invalid message size from client %d: %zu\n", client, size);
        return;
    }

    memcpy(&receivedState, data, sizeof(PlayerState));

    // Update player's position
    player = getPlayerByID(clientPlayers[client]);
    if (player == NULL)
        return;

    player->x = receivedState.player.x;
    player->y = receivedState.player.y;
    player->color = receivedState.player.color;
}

static void sendServerState(void)
{
    NetMessage message;
    Entity* player;

    if (serverClientsCount() == 0)
        return;

    memset(&message, 0, sizeof(NetMessage));
    message.type = GAME_STATE;

    // Loops over active players
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        player = &game.players[i];
        if (!player->active)
            continue;

        message.data.gameState.players[message.data.gameState.numPlayers] = *player;
        message.data.gameState.numPlayers++;
    }

    // Broadcast the game state to each connected peer
    serverBroadcast(&message, sizeof(NetMessage));
}
