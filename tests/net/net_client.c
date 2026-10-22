/*
* Headless client, driven like the game (menu -> net.c -> netsync.c -> client.c).
* Usage: net_client <scenario>, exits with 0 if the scenario behaves as expected:
*   connect  two sessions in a row with a server running; moves the local player to x=123
*   invalid  invalid IP addresses are refused without leaving a session behind
*   full     the server is full: the attempt fails once, without a disconnection
*   timeout  no server: the attempt fails once after the timeout
*/

#include "networking/net.h"
#include "net_test.h"
#include "../test.h"


App app;
Game game;
HEADLESS_DRAWING_STUBS

extern void initNetworkSync(void);
extern void sendGameUpdate(void);
extern Entity* getLocalPlayer(void);
extern Entity* getPlayerByID(int id);

static int connectOk = 0, connectFail = 0, disconnected = 0;
static void onConnectOk(void) { connectOk++; }
static void onConnectFail(void) { connectFail++; }
static void onDisconnect(void) { disconnected++; }


// What initGame and the Client button do
static int startClient(const char* ipAddress)
{
    memset(&game, 0, sizeof(Game));
    connectOk = connectFail = disconnected = 0;

    initNetworkSync();
    setHostType(NET_HOST_CLIENT);
    setAfterConnect(onConnectOk, onConnectFail);
    setAfterDisconnect(onDisconnect);

    if (netCreateHost((char*)ipAddress, testPort()) < 0)
    {
        netDisposeHost();
        return -1;
    }

    return 0;
}

// What doGame does every frame; a non negative x moves the local player there
static void runFrames(int milliseconds, float x)
{
    for (Uint32 start = SDL_GetTicks(); SDL_GetTicks() - start < (Uint32)milliseconds; usleep(16000))
    {
        doNetworkingBefore();
        if (getLocalPlayer() != NULL && x >= 0)
            getLocalPlayer()->x = x;
        sendGameUpdate();
        doNetworkingAfter();
    }
}

int main(int argc, char** argv)
{
    const char* scenario = argc > 1 ? argv[1] : "";

    setvbuf(stdout, NULL, _IOLBF, 0);
    enet_initialize();

    if (strcmp(scenario, "connect") == 0)
    {
        for (int session = 0; session < 2; session++)
        {
            CHECK(startClient("") == 0);
            runFrames(1000, 123);

            // Connected once, with the local player and the server's player (P0) received
            CHECK(connectOk == 1 && connectFail == 0 && disconnected == 0);
            CHECK(getLocalPlayer() != NULL);
            CHECK(getPlayerByID(0) != NULL && getPlayerByID(0)->x == 50);

            // Esc
            netDisposeHost();
            CHECK(disconnected == 1);
            CHECK(getHostType() == NET_HOST_NONE);
        }
    }
    else if (strcmp(scenario, "invalid") == 0)
    {
        CHECK(startClient("1.2.3") == -1);
        CHECK(startClient("1..2.3.4") == -1);
        CHECK(connectOk == 0 && connectFail == 0 && disconnected == 0);
        CHECK(getHostType() == NET_HOST_NONE);

        // Nothing was left behind
        CHECK(startClient("") == 0);
        netDisposeHost();
    }
    else if (strcmp(scenario, "full") == 0)
    {
        CHECK(startClient("") == 0);
        runFrames(1500, -1);
        CHECK(connectOk == 0 && connectFail == 1 && disconnected == 0);
        CHECK(getHostType() == NET_HOST_NONE);
    }
    else if (strcmp(scenario, "timeout") == 0)
    {
        CHECK(startClient("") == 0);
        runFrames(5600, -1);
        CHECK(connectOk == 0 && connectFail == 1 && disconnected == 0);
        CHECK(getHostType() == NET_HOST_NONE);
    }
    else
    {
        printf("Unknown scenario: %s\n", scenario);
        return 2;
    }

    printf("client %s: %s\n", scenario, TEST_RESULT == 0 ? "ok" : "FAILED");
    enet_deinitialize();

    return TEST_RESULT;
}
