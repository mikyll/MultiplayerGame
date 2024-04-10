#pragma once

#include "common.h"

typedef struct Entity Entity;

typedef struct {
	void (*logic)(void);
	void (*draw)(void);
} Delegate;

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
	int state;
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
} Game;