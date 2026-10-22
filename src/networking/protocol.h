#pragma once

#include "../core/common.h"


// Sent by clients when connecting: the server refuses other versions.
// Bump it on every incompatible change of the messages (0 is used by clients older than versioning).
#define PROTOCOL_VERSION		1

// Serialized NetPlayer: id (1) + x, y (4 + 4) + r, g, b (3) + health (1)
#define NET_PLAYER_SIZE			13
// Largest serialized message: GAME_STATE with every player
#define NET_MAX_MESSAGE_SIZE	(2 + MAX_PLAYERS * NET_PLAYER_SIZE)

// ENet channels: state updates are sent often and superseded by the next one, so they travel
// unreliable (lost or late packets are dropped); everything else is reliable
#define NET_CHANNEL_RELIABLE	0
#define NET_CHANNEL_STATE		1
#define NET_CHANNELS			2


int isStateMessage(NetMessageType type);
size_t writeMessage(const NetMessage* message, Uint8* buffer, size_t capacity);
int readMessage(const Uint8* buffer, size_t size, NetMessage* message);
