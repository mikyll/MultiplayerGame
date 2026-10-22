#pragma once

#include "../core/common.h"


// Serialized NetPlayer: id (1) + x, y (4 + 4) + r, g, b (3) + health (1)
#define NET_PLAYER_SIZE			13
// Largest serialized message: GAME_STATE with every player
#define NET_MAX_MESSAGE_SIZE	(2 + MAX_PLAYERS * NET_PLAYER_SIZE)


size_t writeMessage(const NetMessage* message, Uint8* buffer, size_t capacity);
int readMessage(const Uint8* buffer, size_t size, NetMessage* message);
