#include "game.h"

Game game;

void resetGame();
static void logic();
static void draw();

void initGame()
{
	memset(&game, 0, sizeof(Game));

	// Set game logic & draw functions
	app.delegate.logic = logic;
	app.delegate.draw = draw;

	resetGame();
}

void resetGame()
{
	Entity* e;

	// Clear players linked list
	while (game.playersHead.next)
	{
		e = game.playersHead.next;
		game.playersHead.next = e->next;
		free(e);
	}

	game.playersTail = &game.playersHead;
	game.playersCount = 0;

	// Create players
	createPlayer((DEFAULT_SCREEN_WIDTH / 2) - PLAYER_SIZE, (DEFAULT_SCREEN_HEIGHT / 2) - PLAYER_SIZE);
	createPlayer(100, 100);
	createPlayer(100, 200);
}

static void logic()
{
	doPlayers();
}

static void draw()
{
	drawPlayers();
}