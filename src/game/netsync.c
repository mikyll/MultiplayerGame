#include "netsync.h"

/*
* Game side of the network protocol: turns connection events and received messages
* into game state changes, and game state into messages. The networking layer only
* transports the data, and validates it while deserializing.
*/


static int onServerStart(void);
static int onClientJoin(int client);
static void onClientLeave(int client);
static void onClientMessage(int client, const NetMessage* message);
static void sendServerState(void);
static int onServerMessage(const NetMessage* message);
static void sendClientState(void);


static int clientPlayers[MAX_SERVER_PEERS]; // Player ID of each connected client, by client slot


void initNetworkSync(void)
{
    ServerHandlers serverHandlers = { onServerStart, onClientJoin, onClientLeave, onClientMessage };
    ClientHandlers clientHandlers = { onServerMessage };

    for (int i = 0; i < MAX_SERVER_PEERS; i++)
        clientPlayers[i] = -1;

    setServerHandlers(&serverHandlers);
    setClientHandlers(&clientHandlers);
}

/*
* Runs after the game logic: sends this instance's view of the game.
*/
void sendGameUpdate(void)
{
    if (getHostType() == NET_HOST_SERVER)
        sendServerState();
    else if (getHostType() == NET_HOST_CLIENT)
        sendClientState();
}

static NetPlayer toNetPlayer(const Entity* player)
{
    NetPlayer netPlayer;

    memset(&netPlayer, 0, sizeof(NetPlayer));
    netPlayer.id = player->id;
    netPlayer.x = player->x;
    netPlayer.y = player->y;
    netPlayer.color = player->color;
    netPlayer.health = player->health;

    return netPlayer;
}

static Entity fromNetPlayer(const NetPlayer* netPlayer)
{
    Entity player;

    memset(&player, 0, sizeof(Entity));
    player.id = netPlayer->id;
    player.w = PLAYER_SIZE;
    player.h = PLAYER_SIZE;
    player.x = netPlayer->x;
    player.y = netPlayer->y;
    player.color = netPlayer->color;
    player.health = netPlayer->health;

    return player;
}

static int collectPlayers(NetPlayer* players)
{
    int numPlayers = 0;

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (game.players[i].active)
            players[numPlayers++] = toNetPlayer(&game.players[i]);
    }

    return numPlayers;
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
    message.data.connectOK.numPlayers = collectPlayers(message.data.connectOK.players);

    serverSend(client, &message);

    // Broadcast PLAYER_JOINED to each player
    memset(&message, 0, sizeof(NetMessage));
    message.type = PLAYER_JOINED;
    message.data.playerJoined.newPlayer = toNetPlayer(getPlayerByID(id));

    serverBroadcast(&message);

    return 0;
}

static void onClientLeave(int client)
{
    NetMessage message;
    int id = clientPlayers[client];

    clientPlayers[client] = -1;

    if (getPlayerByID(id) == NULL)
        return;

    // Broadcast PLAYER_LEFT to each player
    memset(&message, 0, sizeof(NetMessage));
    message.type = PLAYER_LEFT;
    message.data.playerLeft.id = id;

    serverBroadcast(&message);

    // Destroy player entity
    destroyPlayerByID(id);
}

static void onClientMessage(int client, const NetMessage* message)
{
    const NetPlayer* state;
    Entity* player;

    // Clients only send their own state
    if (message->type != PLAYER_STATE)
    {
        printf("Unexpected message of type %d from client %d\n", message->type, client);
        return;
    }

    // Update player's position (the id in the message is ignored: the sender is known)
    player = getPlayerByID(clientPlayers[client]);
    if (player == NULL)
        return;

    state = &message->data.playerState.player;
    player->x = state->x;
    player->y = state->y;
    player->color = state->color;
}

static void sendServerState(void)
{
    NetMessage message;

    if (serverClientsCount() == 0)
        return;

    memset(&message, 0, sizeof(NetMessage));
    message.type = GAME_STATE;
    message.data.gameState.numPlayers = collectPlayers(message.data.gameState.players);

    // Broadcast the game state to each connected peer
    serverBroadcast(&message);
}


// Client

static int onServerMessage(const NetMessage* message)
{
    Entity* player;
    int id;

    switch (message->type)
    {
        case CONNECT_OK:
        {
            const ConnectOK* connectOK = &message->data.connectOK;

            // Spawn missing players
            for (int i = 0; i < connectOK->numPlayers; i++)
            {
                if (getPlayerByID(connectOK->players[i].id) == NULL)
                {
                    id = spawnPlayer(fromNetPlayer(&connectOK->players[i]));

                    // Set local player
                    if (connectOK->localID == connectOK->players[i].id)
                    {
                        setLocalPlayer(getPlayerByID(id));
                    }
                }
            }

            // Test
            printf("CONNECT_OK: %d\n", connectOK->localID);

            return 0;
        }
        case PLAYER_JOINED:
        {
            const NetPlayer* newPlayer = &message->data.playerJoined.newPlayer;

            if (getPlayerByID(newPlayer->id) == NULL)
            {
                spawnPlayer(fromNetPlayer(newPlayer));
            }

            // Test
            printf("PLAYER_JOINED: %d\n", newPlayer->id);

            return 0;
        }
        case PLAYER_LEFT:
        {
            id = message->data.playerLeft.id;

            if (getPlayerByID(id) != NULL)
            {
                destroyPlayerByID(id);
            }

            // Test
            printf("PLAYER_LEFT: %d\n", id);

            return 0;
        }
        case GAME_STATE:
        {
            const GameState* gameState = &message->data.gameState;

            // Update remote players: the local one is driven by input, the server only echoes
            // back an outdated copy of its position
            for (int i = 0; i < gameState->numPlayers; i++)
            {
                player = getPlayerByID(gameState->players[i].id);
                if (player != NULL && player != getLocalPlayer())
                {
                    player->x = gameState->players[i].x;
                    player->y = gameState->players[i].y;
                    player->color = gameState->players[i].color;
                }
            }

            return 0;
        }
        default:
        {
            printf("Unexpected message of type %d from the server\n", message->type);
            return -1;
        }
    }
}

static void sendClientState(void)
{
    NetMessage message;
    Entity* player = getLocalPlayer();

    // Nothing to send until CONNECT_OK assigns the local player
    if (player == NULL)
        return;

    memset(&message, 0, sizeof(NetMessage));
    message.type = PLAYER_STATE;
    message.data.playerState.player = toNetPlayer(player);

    // Send local player position
    clientSend(&message);
}
