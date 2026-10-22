#pragma once

#include "common.h"

typedef struct Entity Entity;
typedef struct Texture Texture;

typedef struct {
	void (*logic)(float deltaTime);
	void (*draw)(void);
} Delegate;

struct Texture {
	char name[MAX_TEXTURE_NAME_LENGTH];
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
	int keyboard[SDL_NUM_SCANCODES];
	Mouse mouse;
	Texture textureHead, * textureTail;
} App;

struct Entity {
	int id;
	float w, h;
	float x, y;
	float dx, dy;
	int health;
	int heading;
	SDL_Color color;
	int active;
};

typedef struct {
	Entity players[MAX_PLAYERS]; // Indexed by player ID
	Entity* localPlayer;         // Player controlled by this instance, NULL if none
	int playersCount;
	int tick;
} Game;

// Networking

/*
* Messages exchanged between server and clients. They are serialized field by field
* (see networking/protocol.c), so their in-memory layout never reaches the network.
*/

// Player fields sent over the network
typedef struct {
	int id;
	float x, y;
	SDL_Color color; // Alpha is not sent
	int health;
} NetPlayer;

typedef enum {
	DENY_SERVER_FULL,
	DENY_VERSION_MISMATCH,
} DenyReason;

typedef struct {
	int localID; // ID of the new player
	NetPlayer players[MAX_PLAYERS];
	int numPlayers;
} ConnectOK;

typedef struct {
	DenyReason reason;
} ConnectDenied;

typedef struct {
	NetPlayer newPlayer;
} PlayerJoined;

typedef struct {
	int id;
} PlayerLeft;

typedef struct {
	NetPlayer players[MAX_PLAYERS];
	int numPlayers;
} GameState;

typedef struct {
	NetPlayer player; // The server ignores the id: it knows who sent the message
} PlayerState;

typedef enum {
	CONNECT_OK,
	CONNECT_DENIED,
	PLAYER_JOINED,
	PLAYER_LEFT,
	GAME_STATE,
	PLAYER_STATE,
} NetMessageType;

typedef struct {
	NetMessageType type;
	union {
		ConnectOK connectOK;
		ConnectDenied connectDenied;
		PlayerJoined playerJoined;
		PlayerLeft playerLeft;
		GameState gameState;
		PlayerState playerState;
	} data;
} NetMessage;

/*
* Game-side handlers of server events: the server only manages connections,
* the game decides what they mean. A client is identified by its slot in [0, MAX_SERVER_PEERS).
*/
typedef struct {
	int (*onStart)(void);                                               // 0 to start, -1 to abort
	int (*onClientJoin)(int client);                                    // 0 to accept, -1 to refuse
	void (*onClientLeave)(int client);
	void (*onClientMessage)(int client, const NetMessage* message);
} ServerHandlers;

/*
* Game-side handler of client events: the client only manages the connection to the server,
* the game applies the messages it receives.
*/
typedef struct {
	int (*onMessage)(const NetMessage* message);                        // 0 if handled, -1 if invalid
} ClientHandlers;
