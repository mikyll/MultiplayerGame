/*
* Unit tests of networking/protocol.c: round trips, wire format and malformed input.
*/

#include "networking/protocol.h"
#include "test.h"


static NetPlayer makePlayer(int id, float x, float y)
{
    NetPlayer player;

    memset(&player, 0, sizeof(NetPlayer));
    player.id = id;
    player.x = x;
    player.y = y;
    player.color.r = 10 + id;
    player.color.g = 20 + id;
    player.color.b = 30 + id;
    player.color.a = 255;
    player.health = 5;

    return player;
}

static int samePlayer(const NetPlayer* a, const NetPlayer* b)
{
    return a->id == b->id && a->x == b->x && a->y == b->y &&
        a->color.r == b->color.r && a->color.g == b->color.g && a->color.b == b->color.b &&
        a->color.a == 255 && a->health == b->health;
}

// Serializes and deserializes message, returning the number of bytes it takes on the wire
static size_t roundTrip(const NetMessage* message, NetMessage* decoded)
{
    Uint8 buffer[NET_MAX_MESSAGE_SIZE];
    size_t size = writeMessage(message, buffer, sizeof(buffer));

    CHECK(size > 0);
    CHECK(readMessage(buffer, size, decoded) == 0);
    CHECK(decoded->type == message->type);

    return size;
}

static void testRoundTrips(void)
{
    NetMessage message, decoded;

    // GAME_STATE with every player: the largest message
    memset(&message, 0, sizeof(NetMessage));
    message.type = GAME_STATE;
    message.data.gameState.numPlayers = MAX_PLAYERS;
    for (int i = 0; i < MAX_PLAYERS; i++)
        message.data.gameState.players[i] = makePlayer(i, 12.5f * i, -3.25f + i);
    CHECK(roundTrip(&message, &decoded) == NET_MAX_MESSAGE_SIZE);
    CHECK(decoded.data.gameState.numPlayers == MAX_PLAYERS);
    for (int i = 0; i < MAX_PLAYERS; i++)
        CHECK(samePlayer(&decoded.data.gameState.players[i], &message.data.gameState.players[i]));

    // CONNECT_OK
    memset(&message, 0, sizeof(NetMessage));
    message.type = CONNECT_OK;
    message.data.connectOK.localID = 2;
    message.data.connectOK.numPlayers = 3;
    for (int i = 0; i < 3; i++)
        message.data.connectOK.players[i] = makePlayer(i, 50, 50);
    CHECK(roundTrip(&message, &decoded) == 3 + 3 * NET_PLAYER_SIZE);
    CHECK(decoded.data.connectOK.localID == 2 && decoded.data.connectOK.numPlayers == 3);
    CHECK(samePlayer(&decoded.data.connectOK.players[2], &message.data.connectOK.players[2]));

    // CONNECT_DENIED
    memset(&message, 0, sizeof(NetMessage));
    message.type = CONNECT_DENIED;
    message.data.connectDenied.reason = DENY_VERSION_MISMATCH;
    CHECK(roundTrip(&message, &decoded) == 2);
    CHECK(decoded.data.connectDenied.reason == DENY_VERSION_MISMATCH);

    // PLAYER_JOINED
    memset(&message, 0, sizeof(NetMessage));
    message.type = PLAYER_JOINED;
    message.data.playerJoined.newPlayer = makePlayer(1, 615.0f, 455.0f);
    CHECK(roundTrip(&message, &decoded) == 1 + NET_PLAYER_SIZE);
    CHECK(samePlayer(&decoded.data.playerJoined.newPlayer, &message.data.playerJoined.newPlayer));

    // PLAYER_LEFT
    memset(&message, 0, sizeof(NetMessage));
    message.type = PLAYER_LEFT;
    message.data.playerLeft.id = 3;
    CHECK(roundTrip(&message, &decoded) == 2);
    CHECK(decoded.data.playerLeft.id == 3);

    // PLAYER_STATE
    memset(&message, 0, sizeof(NetMessage));
    message.type = PLAYER_STATE;
    message.data.playerState.player = makePlayer(0, 0.5f, 1e6f);
    CHECK(roundTrip(&message, &decoded) == 1 + NET_PLAYER_SIZE);
    CHECK(samePlayer(&decoded.data.playerState.player, &message.data.playerState.player));
}

static void testWireFormat(void)
{
    // PLAYER_JOINED of P1 at (1.0, -2.0), color (11, 21, 31), health 5: big-endian IEEE 754 floats
    const Uint8 expected[] = {
        PLAYER_JOINED,
        1,
        0x3F, 0x80, 0x00, 0x00,
        0xC0, 0x00, 0x00, 0x00,
        11, 21, 31,
        5
    };
    Uint8 buffer[NET_MAX_MESSAGE_SIZE];
    NetMessage message;

    memset(&message, 0, sizeof(NetMessage));
    message.type = PLAYER_JOINED;
    message.data.playerJoined.newPlayer = makePlayer(1, 1.0f, -2.0f);

    CHECK(writeMessage(&message, buffer, sizeof(buffer)) == sizeof(expected));
    CHECK(memcmp(buffer, expected, sizeof(expected)) == 0);
}

static void testMalformed(void)
{
    Uint8 buffer[NET_MAX_MESSAGE_SIZE + 1];
    NetMessage message, decoded;
    size_t size;

    memset(&message, 0, sizeof(NetMessage));
    message.type = GAME_STATE;
    message.data.gameState.numPlayers = 2;
    message.data.gameState.players[0] = makePlayer(0, 1, 1);
    message.data.gameState.players[1] = makePlayer(1, 2, 2);
    size = writeMessage(&message, buffer, sizeof(buffer));
    CHECK(size > 0);

    // Every truncation and a trailing byte are rejected
    for (size_t length = 0; length < size; length++)
        CHECK(readMessage(buffer, length, &decoded) == -1);
    buffer[size] = 0;
    CHECK(readMessage(buffer, size + 1, &decoded) == -1);

    // Players count above MAX_PLAYERS
    buffer[1] = MAX_PLAYERS + 1;
    CHECK(readMessage(buffer, size, &decoded) == -1);
    buffer[1] = 2;

    // Player ID out of range
    buffer[2] = MAX_PLAYERS;
    CHECK(readMessage(buffer, size, &decoded) == -1);
    buffer[2] = 0;
    CHECK(readMessage(buffer, size, &decoded) == 0);

    // NaN and infinity coordinates
    {
        const Uint8 nan[] = { 0x7F, 0xC0, 0x00, 0x00 };
        const Uint8 inf[] = { 0x7F, 0x80, 0x00, 0x00 };

        memcpy(&buffer[3], nan, 4);
        CHECK(readMessage(buffer, size, &decoded) == -1);
        memcpy(&buffer[3], inf, 4);
        CHECK(readMessage(buffer, size, &decoded) == -1);
    }

    // Unknown type and deny reason
    {
        const Uint8 unknownType[] = { 200 };
        const Uint8 unknownReason[] = { CONNECT_DENIED, 99 };
        const Uint8 badLocalID[] = { CONNECT_OK, MAX_PLAYERS, 0 };
        const Uint8 badLeftID[] = { PLAYER_LEFT, MAX_PLAYERS };

        CHECK(readMessage(unknownType, sizeof(unknownType), &decoded) == -1);
        CHECK(readMessage(unknownReason, sizeof(unknownReason), &decoded) == -1);
        CHECK(readMessage(badLocalID, sizeof(badLocalID), &decoded) == -1);
        CHECK(readMessage(badLeftID, sizeof(badLeftID), &decoded) == -1);
    }
}

static void testWriteErrors(void)
{
    Uint8 buffer[NET_MAX_MESSAGE_SIZE];
    NetMessage message;

    // Invalid content
    memset(&message, 0, sizeof(NetMessage));
    message.type = GAME_STATE;
    message.data.gameState.numPlayers = MAX_PLAYERS + 1;
    CHECK(writeMessage(&message, buffer, sizeof(buffer)) == 0);

    message.data.gameState.numPlayers = 1;
    message.data.gameState.players[0] = makePlayer(MAX_PLAYERS, 0, 0);
    CHECK(writeMessage(&message, buffer, sizeof(buffer)) == 0);

    message.type = PLAYER_LEFT;
    message.data.playerLeft.id = -1;
    CHECK(writeMessage(&message, buffer, sizeof(buffer)) == 0);

    message.type = (NetMessageType)200;
    CHECK(writeMessage(&message, buffer, sizeof(buffer)) == 0);

    // Not enough space
    memset(&message, 0, sizeof(NetMessage));
    message.type = PLAYER_STATE;
    message.data.playerState.player = makePlayer(0, 0, 0);
    CHECK(writeMessage(&message, buffer, NET_PLAYER_SIZE) == 0);
    CHECK(writeMessage(&message, buffer, NET_PLAYER_SIZE + 1) == NET_PLAYER_SIZE + 1);
}

int main(void)
{
    testRoundTrips();
    testWireFormat();
    testMalformed();
    testWriteErrors();

    return TEST_RESULT;
}
