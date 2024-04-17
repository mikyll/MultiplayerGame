#pragma once

#include "common.h"

typedef struct Entity Entity;
typedef struct Texture Texture;

typedef struct {
	void (*logic)(void);
	void (*draw)(void);
} Delegate;

struct Texture {
	char name[MAX_NAME_LENGTH];
	SDL_Texture* texture;
	Texture* next;
};

typedef struct {
	int x;
	int y;
	int button[MAX_MOUSE_BUTTONS];
	int wheel;
} Mouse;

typedef struct {
	SDL_Renderer* renderer;
	SDL_Window* window;
	Delegate delegate;
	int keyboard[MAX_KEYBOARD_KEYS];
	Mouse mouse;
	Texture textureHead, * textureTail;
} App;

struct Entity {
	int id;
	float w, h;
	float x, y;
	float dx, dy;
	int heading;
	SDL_Color color;
	Entity* next;
};

typedef struct {
	Entity playersHead, * playersTail;
	int playersCount;
	int tick;
} Game;

// Networking
typedef struct {
	int localID;
	Entity players[MAX_PLAYERS];
	int numPlayers;
} ConnectOK;

typedef struct {
	int id;
	char message[64];
} ConnectDenied;

typedef struct {
	int type;
	Entity players[MAX_PLAYERS];
	int numPlayers;
} GameState;

typedef struct {
	Entity player;
	//ENetPeer peer;
} PlayerState;

typedef enum {
	CONNECT_OK,
	CONNECT_DENIED,
	GAME_STATE,
	PLAYER_STATE,
} NetMessageType;

typedef struct {
	NetMessageType type;
	union {
		ConnectOK connectOK;
		ConnectDenied connectDenied;
		GameState gameState;
		PlayerState playerState;
	} data;
} NetMessage;