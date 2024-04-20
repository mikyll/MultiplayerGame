#include "game.h"

Game game;

void initGame();
static void resetGame();
static void doGame(float deltaTime);
static void drawGame();

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
}

void showGame()
{
	// Set game logic & draw functions
	app.delegate.logic = doGame;
	app.delegate.draw = drawGame;
}

static void doGame(float deltaTime)
{
	// Receive network input
	doNetworkingBefore();

	// Quit the game
	if (app.keyboard[SDL_SCANCODE_ESCAPE])
	{
		app.keyboard[SDL_SCANCODE_ESCAPE] = 0;

		netDisposeHost();
	}

	doPlayers(deltaTime);

	// Send network update
	doNetworkingAfter();
}

static void drawGame()
{
	Entity* player;
	char buffer[64];
	int i;
	float size = 0.5f;

	drawPlayers();

	drawTextScaled(5, 5, 0.75, 0, 0, 0, TEXT_LEFT, hostTypeToString(getHostType()));

	secure_sprintf(buffer, sizeof(buffer), "Players count: %d", game.playersCount);
	drawTextScaled(DEFAULT_WINDOW_WIDTH - 5, 50, 0.5, 0, 0, 0, TEXT_RIGHT, buffer);

	for (i = 0, player = game.playersHead.next; player != NULL; player = player->next, i++)
	{
		secure_sprintf(buffer, sizeof(buffer), "P%d (%3.f,%3.f)", player->id, player->x, player->y);
		drawTextScaled(DEFAULT_WINDOW_WIDTH - 5, 70 + i * GLYPH_HEIGHT * size + 5, size, player->color.r, player->color.g, player->color.b, TEXT_RIGHT, buffer);
	}
}