#include "net.h"

ENetAddress address;
ENetHost* host;
ENetHost* client;

int hostType = NET_HOST_NONE;

static int (*create)(void);
static void (*dispose)(void);
static void (*netReceive)(void);
static void (*netSend)(void);

void initEnet()
{
    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
}

void cleanupEnet()
{
    enet_host_destroy(host);

    enet_deinitialize();
}

void setHostType(int type)
{
    if (type == NET_HOST_SERVER)
    {
        hostType = NET_HOST_SERVER;
        create = createServer;
        dispose = destroyServer;
        netReceive = serverBefore;
        netSend = serverAfter;
    }
    if (type == NET_HOST_CLIENT)
    {
        hostType = NET_HOST_CLIENT;
        create = createClient;
        dispose = destroyClient;
        netReceive = clientBefore;
        netSend = clientAfter;
    }
}

int getHostType()
{
    return hostType;
}
char* hostTypeToString(int type)
{
    switch (type)
    {
    case NET_HOST_NONE:
        return "None";
    case NET_HOST_SERVER:
        return "Server";
    case NET_HOST_CLIENT:
        return "Client";
    }
}

int createHost()
{
    if (create != NULL)
        create();
}

void disposeHost()
{
    if (dispose != NULL)
        dispose();
}

void doNetworkingBefore()
{
    if (netReceive != NULL)
        netReceive();
}

void doNetworkingAfter()
{
    if (netSend != NULL)
        netSend();
}


// Client -> Server
/*void sendInput()
{
    // Create a reliable packet of size 7 containing "packet\0"
    ENetPacket* packet = enet_packet_create("packet",
        strlen("packet") + 1,
        ENET_PACKET_FLAG_RELIABLE);

    // Extend the packet so and append the string "foo", so it now
    // contains "packetfoo\0"
    enet_packet_resize(packet, strlen("packetfoo") + 1);
    strcpy(&packet->data[strlen("packet")], "foo");
    // Send the packet to the peer over channel id 0.
    // One could also broadcast the packet by
    // enet_host_broadcast (host, 0, packet);
    enet_peer_send(peer, 0, packet);

    // ...

    // One could just use enet_host_service() instead.
    enet_host_flush(host);
}*/

// Server -> Clients
void sendGameState()
{

}

void doNetwork()
{
    ENetEvent event;

    // Wait up to 1000 milliseconds for an event
    while (enet_host_service(client, &event, 1000) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            printf("A new client connected from %x:%u.\n",
                event.peer->address.host,
                event.peer->address.port);

            // Store any relevant client information here
            event.peer->data = "Client information";
            break;
        case ENET_EVENT_TYPE_RECEIVE:
            printf("A packet of length %u containing %s was received from %s on channel %u.\n",
                event.packet->dataLength,
                event.packet->data,
                event.peer->data,
                event.channelID);
            // Clean up the packet now that we're done using it
            enet_packet_destroy(event.packet);

            break;

        case ENET_EVENT_TYPE_DISCONNECT:
            printf("%s disconnected.\n", event.peer->data);
            // Reset the peer's client information
            event.peer->data = NULL;
        }
    }
}

/*GamePacket buildGamePacket()
{
    GamePacket gamePacket;
    memset(&gamePacket, 0, sizeof(GamePacket));

    Entity* player;
    int i = 0;
    for (player = game.playersHead.next; player != NULL; player = player->next)
    {
        Entity p = *player;
        player->next = NULL;

        gamePacket.players[i] = p;
        gamePacket.numPlayers++;
    }

    // Set tick
    //gamePacket.tick;

    return gamePacket;
}*/

