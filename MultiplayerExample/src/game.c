#include "game.h"

Game game;

void initGame();
static void resetGame();
static void logic();
static void draw();

void initGame()
{
	memset(&game, 0, sizeof(Game));

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
	/*spawnPlayer((DEFAULT_WINDOW_WIDTH / 2) - PLAYER_SIZE, (DEFAULT_WINDOW_HEIGHT / 2) - PLAYER_SIZE);
	spawnPlayer(100, 100);
	spawnPlayer(100, 200);*/
}

void showGame()
{
	// Set game logic & draw functions
	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void logic()
{
	// Receive network input
	// TODO
	// Networking Receive (Client: receive game state; server: receive peer Inputs)
	//doNetworkingReceive();
	doNetworkingBefore();

	if (app.keyboard[SDL_SCANCODE_ESCAPE])
	{
		app.keyboard[SDL_SCANCODE_ESCAPE] = 0;

		disposeHost();

		showMenu();
	}

	doPlayers();


	// Send network update
	// TODO
	// Networking Send
	//doNetworkingSend();
	doNetworkingAfter();
}

static void draw()
{
	Entity* player;
	int i;

	drawPlayers();

	drawTextScaled(5, 5, 0.75, 0, 0, 0, TEXT_LEFT, hostTypeToString(getHostType()));

	char buffer[64];
	sprintf_s(buffer, sizeof(buffer), "Players count: %d", game.playersCount);
	drawTextScaled(DEFAULT_WINDOW_WIDTH - 5, 5, 0.5, 0, 0, 0, TEXT_RIGHT, buffer);

	
	i = 0;
	for (player = game.playersHead.next; player != NULL; player = player->next)
	{
		char buffer[64];
		float size = 0.5f;
		sprintf_s(buffer, sizeof(buffer), "P%d (%3.f,%3.f)", player->id, player->x, player->y);
		drawTextScaled(DEFAULT_WINDOW_WIDTH - 5, 30 + i * GLYPH_HEIGHT * size + 5, size, player->color.r, player->color.g, player->color.b, TEXT_RIGHT, buffer);
		i++;
	}
}