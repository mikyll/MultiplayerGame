/*
* Unit tests of game/player.c: ID allocation, lookup, movement, removal and drawing order.
*/

#include "game/player.h"
#include "test.h"


App app;
Game game;

// Drawing stubs: blitRect records the order players are drawn in, using the color alpha as a tag
static int drawOrder[MAX_PLAYERS * 2];
static int drawn = 0;

void blitRect(int x, int y, int w, int h, SDL_Color c) { drawOrder[drawn++] = c.a; }
void blitRectBorder(int x, int y, int w, int h, SDL_Color c) {}
void drawTextScaled(int x, int y, float size, int r, int g, int b, int align, char* format, ...) {}
int secure_sprintf(char* buffer, size_t bufferSize, const char* format, ...) { return 0; }


static void resetGame(void)
{
    memset(&game, 0, sizeof(Game));
    memset(&app, 0, sizeof(App));
}

static void testIDs(void)
{
    resetGame();

    // IDs are assigned in order, until the game is full
    for (int i = 0; i < MAX_PLAYERS; i++)
        CHECK(spawnPlayerAt(0, 0) == i);
    CHECK(game.playersCount == MAX_PLAYERS);
    CHECK(spawnPlayerAt(0, 0) == -1);
    CHECK(game.playersCount == MAX_PLAYERS);

    // A freed ID is reused
    destroyPlayerByID(1);
    CHECK(game.playersCount == MAX_PLAYERS - 1);
    CHECK(getPlayerByID(1) == NULL);
    CHECK(spawnPlayerAt(0, 0) == 1);

    // Out of range IDs are ignored
    CHECK(getPlayerByID(-1) == NULL);
    CHECK(getPlayerByID(MAX_PLAYERS) == NULL);
    destroyPlayerByID(-5);
    destroyPlayerByID(99);
    CHECK(game.playersCount == MAX_PLAYERS);
}

static void testSpawnFromNetwork(void)
{
    Entity p = { 0 };

    resetGame();

    p.id = 2;
    p.x = 10;
    CHECK(spawnPlayer(p) == 2);
    CHECK(getPlayerByID(2) != NULL && getPlayerByID(2)->x == 10);
    CHECK(game.playersCount == 1);

    // Taken or invalid IDs are refused
    CHECK(spawnPlayer(p) == -1);
    p.id = 99;
    CHECK(spawnPlayer(p) == -1);
    p.id = -1;
    CHECK(spawnPlayer(p) == -1);
    CHECK(game.playersCount == 1);
    CHECK(findFirstAvailableID() == 0);
}

static void testMovement(void)
{
    Entity* player;
    float distance;

    resetGame();
    spawnPlayerAt(100, 100);
    spawnPlayerAt(200, 200);

    // Without a local player, input moves nobody
    app.keyboard[SDL_SCANCODE_D] = 1;
    doPlayers(0.01f);
    CHECK(getPlayerByID(0)->x == 100);
    CHECK(getPlayerByID(1)->x == 200);

    // Only the local player moves, at the same speed diagonally
    setLocalPlayer(getPlayerByID(1));
    app.keyboard[SDL_SCANCODE_W] = 1;
    doPlayers(0.01f);
    player = getPlayerByID(1);
    distance = sqrtf((player->x - 200) * (player->x - 200) + (player->y - 200) * (player->y - 200));
    CHECK(getPlayerByID(0)->x == 100);
    CHECK(fabsf(distance - PLAYER_SPEED * 0.01f) < 1e-3f);
}

static void testHealthRemoval(void)
{
    resetGame();
    setLocalPlayer(getPlayerByID(spawnPlayerAt(0, 0)));
    spawnPlayerAt(0, 0);

    getPlayerByID(0)->health = 0;
    doPlayers(0.01f);
    CHECK(game.playersCount == 1);
    CHECK(getPlayerByID(0) == NULL);
    CHECK(getLocalPlayer() == NULL);
}

static void testDrawingOrder(void)
{
    resetGame();
    for (int i = 0; i < 3; i++)
    {
        spawnPlayerAt(0, 0);
        getPlayerByID(i)->color.a = 10 + i;
    }

    // The local player is drawn last, on top of the others
    setLocalPlayer(getPlayerByID(0));
    drawn = 0;
    drawPlayers();
    CHECK(drawn == 3);
    CHECK(drawOrder[0] == 11 && drawOrder[1] == 12 && drawOrder[2] == 10);
}

int main(void)
{
    testIDs();
    testSpawnFromNetwork();
    testMovement();
    testHealthRemoval();
    testDrawingOrder();

    return TEST_RESULT;
}
