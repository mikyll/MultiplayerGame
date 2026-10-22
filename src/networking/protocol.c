#include "protocol.h"

/*
* Wire format of NetMessage. Every message starts with its type (1 byte), followed by:
*   CONNECT_OK      localID (1), numPlayers (1), players
*   CONNECT_DENIED  reason (1)
*   PLAYER_JOINED   player
*   PLAYER_LEFT     id (1)
*   GAME_STATE      numPlayers (1), players
*   PLAYER_STATE    player
* A player is id (1), x (4), y (4), r (1), g (1), b (1), health (1).
* Multi-byte values are big-endian, floats are sent as their IEEE 754 bits.
*/


typedef struct {
    Uint8* data;
    size_t capacity;
    size_t size;
    int error;
} Writer;

typedef struct {
    const Uint8* data;
    size_t size;
    size_t position;
    int error;
} Reader;


static void writeU8(Writer* writer, Uint8 value)
{
    if (writer->size + 1 > writer->capacity)
    {
        writer->error = 1;
        return;
    }

    writer->data[writer->size++] = value;
}

static void writeU32(Writer* writer, Uint32 value)
{
    writeU8(writer, (Uint8)(value >> 24));
    writeU8(writer, (Uint8)(value >> 16));
    writeU8(writer, (Uint8)(value >> 8));
    writeU8(writer, (Uint8)value);
}

static void writeFloat(Writer* writer, float value)
{
    Uint32 bits;

    memcpy(&bits, &value, sizeof(bits));
    writeU32(writer, bits);
}

static void writePlayer(Writer* writer, const NetPlayer* player)
{
    if (player->id < 0 || player->id >= MAX_PLAYERS)
    {
        writer->error = 1;
        return;
    }

    writeU8(writer, (Uint8)player->id);
    writeFloat(writer, player->x);
    writeFloat(writer, player->y);
    writeU8(writer, player->color.r);
    writeU8(writer, player->color.g);
    writeU8(writer, player->color.b);
    writeU8(writer, (Uint8)MAX(0, MIN(player->health, 255)));
}

static void writePlayers(Writer* writer, const NetPlayer* players, int numPlayers)
{
    if (numPlayers < 0 || numPlayers > MAX_PLAYERS)
    {
        writer->error = 1;
        return;
    }

    writeU8(writer, (Uint8)numPlayers);
    for (int i = 0; i < numPlayers; i++)
        writePlayer(writer, &players[i]);
}

static Uint8 readU8(Reader* reader)
{
    if (reader->position + 1 > reader->size)
    {
        reader->error = 1;
        return 0;
    }

    return reader->data[reader->position++];
}

static Uint32 readU32(Reader* reader)
{
    Uint32 value = 0;

    for (int i = 0; i < 4; i++)
        value = (value << 8) | readU8(reader);

    return value;
}

static float readFloat(Reader* reader)
{
    Uint32 bits = readU32(reader);
    float value;

    memcpy(&value, &bits, sizeof(value));

    // NaN or infinity would corrupt the game state
    if (!isfinite(value))
        reader->error = 1;

    return value;
}

static void readPlayer(Reader* reader, NetPlayer* player)
{
    player->id = readU8(reader);
    player->x = readFloat(reader);
    player->y = readFloat(reader);
    player->color.r = readU8(reader);
    player->color.g = readU8(reader);
    player->color.b = readU8(reader);
    player->color.a = 255;
    player->health = readU8(reader);

    if (player->id >= MAX_PLAYERS)
        reader->error = 1;
}

static int readPlayers(Reader* reader, NetPlayer* players)
{
    int numPlayers = readU8(reader);

    if (numPlayers > MAX_PLAYERS)
    {
        reader->error = 1;
        return 0;
    }

    for (int i = 0; i < numPlayers && !reader->error; i++)
        readPlayer(reader, &players[i]);

    return numPlayers;
}

/*
* Serializes message into buffer.
* Returns the number of bytes written, or 0 if the message is invalid or doesn't fit.
*/
size_t writeMessage(const NetMessage* message, Uint8* buffer, size_t capacity)
{
    Writer writer = { buffer, capacity, 0, 0 };

    writeU8(&writer, (Uint8)message->type);

    switch (message->type)
    {
        case CONNECT_OK:
        {
            if (message->data.connectOK.localID < 0 || message->data.connectOK.localID >= MAX_PLAYERS)
                return 0;

            writeU8(&writer, (Uint8)message->data.connectOK.localID);
            writePlayers(&writer, message->data.connectOK.players, message->data.connectOK.numPlayers);
            break;
        }
        case CONNECT_DENIED:
        {
            writeU8(&writer, (Uint8)message->data.connectDenied.reason);
            break;
        }
        case PLAYER_JOINED:
        {
            writePlayer(&writer, &message->data.playerJoined.newPlayer);
            break;
        }
        case PLAYER_LEFT:
        {
            if (message->data.playerLeft.id < 0 || message->data.playerLeft.id >= MAX_PLAYERS)
                return 0;

            writeU8(&writer, (Uint8)message->data.playerLeft.id);
            break;
        }
        case GAME_STATE:
        {
            writePlayers(&writer, message->data.gameState.players, message->data.gameState.numPlayers);
            break;
        }
        case PLAYER_STATE:
        {
            writePlayer(&writer, &message->data.playerState.player);
            break;
        }
        default:
        {
            return 0;
        }
    }

    return writer.error ? 0 : writer.size;
}

/*
* Deserializes and validates a message.
* Returns 0 on success, -1 if the data is truncated, too long, or holds invalid values.
*/
int readMessage(const Uint8* buffer, size_t size, NetMessage* message)
{
    Reader reader = { buffer, size, 0, 0 };

    memset(message, 0, sizeof(NetMessage));
    message->type = readU8(&reader);

    switch (message->type)
    {
        case CONNECT_OK:
        {
            message->data.connectOK.localID = readU8(&reader);
            message->data.connectOK.numPlayers = readPlayers(&reader, message->data.connectOK.players);

            if (message->data.connectOK.localID >= MAX_PLAYERS)
                return -1;
            break;
        }
        case CONNECT_DENIED:
        {
            message->data.connectDenied.reason = readU8(&reader);

            if (message->data.connectDenied.reason > DENY_VERSION_MISMATCH)
                return -1;
            break;
        }
        case PLAYER_JOINED:
        {
            readPlayer(&reader, &message->data.playerJoined.newPlayer);
            break;
        }
        case PLAYER_LEFT:
        {
            message->data.playerLeft.id = readU8(&reader);

            if (message->data.playerLeft.id >= MAX_PLAYERS)
                return -1;
            break;
        }
        case GAME_STATE:
        {
            message->data.gameState.numPlayers = readPlayers(&reader, message->data.gameState.players);
            break;
        }
        case PLAYER_STATE:
        {
            readPlayer(&reader, &message->data.playerState.player);
            break;
        }
        default:
        {
            return -1;
        }
    }

    // Truncated data, or trailing bytes that don't belong to the message
    if (reader.error || reader.position != size)
        return -1;

    return 0;
}
