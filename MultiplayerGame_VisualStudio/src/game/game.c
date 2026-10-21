#include "game.h"


static void resetGame(void);
static void doGame(float deltaTime);
static void drawGame(void);


Game game;


void initGame(void)
{
	memset(&game, 0, sizeof(Game));

	resetGame();
}

void resetGame(void)
{
	// Clear players
	memset(game.players, 0, sizeof(game.players));
	game.playersCount = 0;

	setLocalPlayer(NULL);
}

void showGame(void)
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

static void drawGame(void)
{
	Entity* player;
	char buffer[64];
	int row = 0;
	float size = 0.5f;

	drawPlayers();

	drawTextScaled(5, 5, 0.75, 0, 0, 0, TEXT_LEFT, hostTypeToString(getHostType()));

	secure_sprintf(buffer, sizeof(buffer), "Players count: %d", game.playersCount);
	drawTextScaled(DEFAULT_WINDOW_WIDTH - 5, 50, 0.5, 0, 0, 0, TEXT_RIGHT, buffer);

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		player = &game.players[i];
		if (!player->active)
			continue;

		secure_sprintf(buffer, sizeof(buffer), "P%d (%3.f,%3.f)", player->id, player->x, player->y);
		drawTextScaled(DEFAULT_WINDOW_WIDTH - 5, 70 + row * GLYPH_HEIGHT * size + 5, size, player->color.r, player->color.g, player->color.b, TEXT_RIGHT, buffer);
		row++;
	}
}

