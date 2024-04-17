#include "client.h"

ENetAddress clientAddress;
ENetAddress serverAddress;
ENetHost* clientHost = NULL;
ENetPeer* serverPeer = NULL;

int createClient()
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

    enet_address_set_host(&serverAddress, HOSTNAME);
    serverAddress.port = PORT;

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
        printf("Connection to %s:%d succeeded.\n", HOSTNAME, PORT);

        // Successfully connected to server

        return EXIT_SUCCESS;
    }

    enet_peer_reset(serverPeer);
    fprintf(stderr, "Connection to %s:%d failed.\n", HOSTNAME, PORT);
    return EXIT_FAILURE;
}

void destroyClient()
{
    if (serverPeer != NULL)
    {
        enet_peer_disconnect_now(serverPeer, 0);
    }

    enet_host_destroy(clientHost);
}

static void handleMessage(ENetPacket* packet)
{
    NetMessage receivedNetMessage;
    memcpy(&receivedNetMessage, packet->data, sizeof(NetMessage));

    switch (receivedNetMessage.type)
    {
    case CONNECT_OK:
        printf("Connect OK message\n");
        // Spawn players
        for (int i = 0; i < receivedNetMessage.data.connectOK.numPlayers; i++)
        {
            int id = spawnPlayer(receivedNetMessage.data.connectOK.players[i]);

            // Set local player
            if (receivedNetMessage.data.connectOK.localID == receivedNetMessage.data.connectOK.players[i].id)
            {
                setLocalPlayer(getPlayerByID(id));
            }
        }

        break;
    case CONNECT_DENIED:
        // TODO
        printf("Connect Denied message: %s\n", receivedNetMessage.data.connectDenied.message);
        
        break;
    case GAME_STATE:
        //printf("Game State message\n");
        
        // update players
        for (int i = 0; i < receivedNetMessage.data.gameState.numPlayers; i++)
        {
            Entity* player = getPlayerByID(receivedNetMessage.data.gameState.players[i].id);
            if (player != NULL)
            {
                player->x = receivedNetMessage.data.gameState.players[i].x;
                player->y = receivedNetMessage.data.gameState.players[i].y;
            }
        }
        break;
    default:
        printf("Unknown message\n");
        break;
    }
}

void clientBefore()
{
    // Receive game state

    ENetEvent event;
    ENetPacket* packet;

    if (clientHost == NULL)
    {
        printf("Client is NULL.\n");
        return;
    }

    while (enet_host_service(clientHost, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:

            handleMessage(event.packet);

            /*
            // Old code
            GameState receivedState;
            memcpy(&receivedState, event.packet->data, sizeof(GameState));

            // update players
            for (int i = 0; i < receivedState.numPlayers; i++)
            {
                Entity* player = getPlayerByID((int)event.peer->data);
                if (player != NULL)
                {
                    player->x = receivedState.players[i].x;
                    player->y = receivedState.players[i].y;
                }
            }*/

            // Clean up the packet now that we're done using it
            enet_packet_destroy(event.packet);
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            destroyClient();
            return;
        }
    }
}

void clientAfter()
{
    if (clientHost == NULL)
        return;

    // Send player position
    Entity* player = getLocalPlayer();
    if (player == NULL)
        return;

    PlayerState playerState;
    memset(&playerState, 0, sizeof(PlayerState));
    
    playerState.player = *player;

    ENetPacket * packet = enet_packet_create(&playerState, sizeof(PlayerState), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(serverPeer, 0, packet);
    enet_host_flush(clientHost);
}