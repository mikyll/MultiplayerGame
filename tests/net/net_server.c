/*
* Headless server, driven like the game (menu -> net.c -> netsync.c -> server.c).
* Usage: net_server <seconds>
* Logs its players every second, so the test script can check what clients sent.
*/

#include "networking/net.h"
#include "net_test.h"


App app;
Game game;
HEADLESS_DRAWING_STUBS

extern void initNetworkSync(void);
extern void sendGameUpdate(void);

static void onConnect(void) {}
static void onFail(void) {}
static void onDisconnect(void) {}


static void logPlayers(const char* prefix)
{
    printf("%s", prefix);
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (game.players[i].active)
            printf(" P%d(x=%.0f)", i, game.players[i].x);
    }
    printf("\n");
}

int main(int argc, char** argv)
{
    int seconds = argc > 1 ? atoi(argv[1]) : 10;
    enet_uint32 lastLog = 0;

    setvbuf(stdout, NULL, _IOLBF, 0);
    enet_initialize();

    // What initGame and the Server button do
    memset(&game, 0, sizeof(Game));
    initNetworkSync();
    setHostType(NET_HOST_SERVER);
    setAfterConnect(onConnect, onFail);
    setAfterDisconnect(onDisconnect);
    if (netCreateHost("", testPort()) < 0)
    {
        printf("server: create failed\n");
        return 1;
    }
    printf("server: up\n");

    // What doGame does every frame
    for (enet_uint32 start = enet_time_get(); enet_time_get() - start < (enet_uint32)seconds * 1000; usleep(16000))
    {
        doNetworkingBefore();
        sendGameUpdate();
        doNetworkingAfter();

        if (enet_time_get() - lastLog >= 1000)
        {
            lastLog = enet_time_get();
            logPlayers("server: players");
        }
    }

    logPlayers("server: final players");
    netDisposeHost();
    enet_deinitialize();

    return 0;
}
