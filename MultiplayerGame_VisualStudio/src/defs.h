#pragma once

#define _TRUNCATE ((size_t)-1)

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define STRNCPY(dest, src, n) secure_strncpy(dest, n, src, _TRUNCATE)

// App
#define APP_TITLE               "Multiplayer Game"
#define APP_VERSION             "-"
#define DEFAULT_WINDOW_WIDTH    640
#define DEFAULT_WINDOW_HEIGHT   480
#define CAP_FPS                 60

// Draw
#define MAX_TEXTURE_NAME_LENGTH  32

// Text
#define MAX_LINE_LENGTH   1024
#define GLYPH_WIDTH       18
#define GLYPH_HEIGHT      29
enum
{
  TEXT_LEFT,
  TEXT_CENTER,
  TEXT_RIGHT
};

// UI
#define MAX_BUTTON_TEXT_LENGTH  64

// Input
#define MAX_MOUSE_BUTTONS   6

// Player
#define PLAYER_SIZE         25.0f
#define PLAYER_BASE_HEALTH  5
#define PLAYER_SPEED        200.0f
#define PLAYER_SPEED_BOOST  2.0f

// Net
#define MAX_PLAYERS       2
#define MAX_SERVER_PEERS  MAX_PLAYERS - 1
#define DEFAULT_HOSTNAME  "localhost"
#define DEFAULT_PORT      8987
enum
{
  NET_HOST_NONE,
  NET_HOST_SERVER,
  NET_HOST_CLIENT,
};

