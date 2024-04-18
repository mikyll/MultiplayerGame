#pragma once

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define STRNCPY(dest, src, n) strncpy_s(dest, n, src, _TRUNCATE)

// App
#define DEFAULT_WINDOW_WIDTH	800
#define DEFAULT_WINDOW_HEIGHT	600
#define CAP_FPS					60

// Draw
#define MAX_NAME_LENGTH			32

// Text
#define MAX_LINE_LENGTH			1024
#define GLYPH_WIDTH				18
#define GLYPH_HEIGHT			29
enum
{
	TEXT_LEFT,
	TEXT_CENTER,
	TEXT_RIGHT
};

// Input
#define MAX_MOUSE_BUTTONS		6

// Player
#define PLAYER_SIZE				25.0f
#define PLAYER_SPEED			200.0f
#define PLAYER_SPEED_BOOST		2.0f

// Net
#define MAX_PLAYERS				4
#define HOSTNAME				"192.168.1.46"
#define PORT					8987
enum
{
	NET_HOST_NONE,
	NET_HOST_SERVER,
	NET_HOST_CLIENT,
};

/*enum
{
	CONNECT_REQUEST = 0x01,
	CONNECT_OK = 0x02,
	CONNECT_REFUSED = 0x03,
	DISCONNECT = 0x04,
	GAME_STATE = 0x05,
	PLAYER_INPUT = 0x06,
};*/