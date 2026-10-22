/*
* Raw ENet probe: opens several connections at once and logs what each receives.
* Usage: net_probe <connections> <milliseconds> [protocol version]
*/

#include "core/common.h"
#include "enet/enet.h"
#include "networking/protocol.h"
#include "net_test.h"

#define MAX_CONNECTIONS 16


static const char* reliability(const ENetPacket* packet)
{
    return (packet->flags & ENET_PACKET_FLAG_RELIABLE) ? "reliable" : "unreliable";
}

int main(int argc, char** argv)
{
    int connections = argc > 1 ? atoi(argv[1]) : 4;
    int duration = argc > 2 ? atoi(argv[2]) : 3000;
    enet_uint32 version = argc > 3 ? (enet_uint32)atoi(argv[3]) : PROTOCOL_VERSION;
    ENetPeer* peers[MAX_CONNECTIONS];
    int gameStates[MAX_CONNECTIONS] = { 0 };
    ENetHost* host;
    ENetAddress address;
    ENetEvent event;
    NetMessage message;

    if (connections < 1 || connections > MAX_CONNECTIONS)
        return 2;

    setvbuf(stdout, NULL, _IOLBF, 0);
    enet_initialize();
    host = enet_host_create(NULL, connections, NET_CHANNELS, 0, 0);
    enet_address_set_host(&address, "127.0.0.1");
    address.port = testPort();

    for (int i = 0; i < connections; i++)
    {
        peers[i] = enet_host_connect(host, &address, NET_CHANNELS, version);
        peers[i]->data = (void*)(intptr_t)i;
    }

    for (enet_uint32 start = enet_time_get(); enet_time_get() - start < (enet_uint32)duration; )
    {
        if (enet_host_service(host, &event, 20) <= 0)
            continue;

        int i = (int)(intptr_t)event.peer->data;

        if (event.type == ENET_EVENT_TYPE_DISCONNECT)
        {
            printf("probe %d: disconnected\n", i);
        }
        else if (event.type == ENET_EVENT_TYPE_RECEIVE)
        {
            if (readMessage(event.packet->data, event.packet->dataLength, &message) < 0)
            {
                printf("probe %d: INVALID message\n", i);
            }
            else if (message.type == CONNECT_OK)
            {
                printf("probe %d: CONNECT_OK P%d on channel %d %s\n", i, message.data.connectOK.localID, event.channelID, reliability(event.packet));
            }
            else if (message.type == CONNECT_DENIED)
            {
                printf("probe %d: CONNECT_DENIED reason %d\n", i, message.data.connectDenied.reason);
            }
            else if (message.type == GAME_STATE && gameStates[i]++ == 0)
            {
                printf("probe %d: GAME_STATE %zu bytes on channel %d %s\n", i, event.packet->dataLength, event.channelID, reliability(event.packet));
            }

            enet_packet_destroy(event.packet);
        }
    }

    for (int i = 0; i < connections; i++)
        enet_peer_disconnect_now(peers[i], 0);
    enet_host_flush(host);
    enet_host_destroy(host);
    enet_deinitialize();

    return 0;
}
