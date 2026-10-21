#include "player.h"


/*
* Players live in game.players: the array index is the player's ID,
* and only slots with active set hold a player.
*/
int findFirstAvailableID(void)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (!game.players[i].active)
			return i;
	}

	return -1;
}

/*
* Adds a player entity with the given ID (e.g. received by the network).
* Returns the player's ID, or -1 if the ID is invalid or already taken.
*/
int spawnPlayer(Entity p)
{
	if (p.id < 0 || p.id >= MAX_PLAYERS || game.players[p.id].active)
	{
		printf("Player creation failed: invalid or taken ID %d.\n", p.id);
		return -1;
	}

	Entity* player = &game.players[p.id];

	// Init player parameters
	memset(player, 0, sizeof(Entity));
	player->id = p.id;
	player->w = p.w;
	player->h = p.h;
	player->x = p.x;
	player->y = p.y;
	player->health = p.health;
	player->color = p.color;
	player->active = 1;

	game.playersCount++;

	return p.id;
}

int spawnPlayerAt(float x, float y)
{
	int id = findFirstAvailableID();
	if (id < 0)
	{
		printf("Player creation failed: no available ID.\n");
		return -1;
	}

	Entity* player = &game.players[id];

	// Init player parameters
	memset(player, 0, sizeof(Entity));
	player->id = id;
	player->w = PLAYER_SIZE;
	player->h = PLAYER_SIZE;
	player->x = x;
	player->y = y;
	player->dx = 0.0f;
	player->dy = 0.0f;
	player->health = PLAYER_BASE_HEALTH;
	SDL_Color color = { rand() % 255, rand() % 255, rand() % 255, 255 };
	player->color = color;
	player->active = 1;

	game.playersCount++;

	return player->id;
}

void destroyPlayer(Entity* player)
{
	if (player == NULL || !player->active)
		return;

	if (player == game.localPlayer)
		game.localPlayer = NULL;

	player->active = 0;
	game.playersCount--;
}

void destroyPlayerByID(int id)
{
	destroyPlayer(getPlayerByID(id));
}

void setLocalPlayer(Entity* player)
{
	game.localPlayer = player;
}

Entity* getLocalPlayer(void)
{
	return game.localPlayer;
}

Entity* getPlayerByID(int id)
{
	if (id < 0 || id >= MAX_PLAYERS || !game.players[id].active)
		return NULL;

	return &game.players[id];
}

void doPlayers(float deltaTime)
{
	Entity* player;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		player = &game.players[i];
		if (!player->active)
			continue;

		player->dx = 0;
		player->dy = 0;

		if (player == game.localPlayer)
		{
			if (app.keyboard[SDL_SCANCODE_W])
				player->dy = -PLAYER_SPEED * deltaTime;
			if (app.keyboard[SDL_SCANCODE_S])
				player->dy = PLAYER_SPEED * deltaTime;
			if (app.keyboard[SDL_SCANCODE_A])
				player->dx = -PLAYER_SPEED * deltaTime;
			if (app.keyboard[SDL_SCANCODE_D])
				player->dx = PLAYER_SPEED * deltaTime;

			// Diagonal movement: keep the same speed as along a single axis
			if (player->dx != 0 && player->dy != 0)
			{
				player->dx *= 0.70710678f; // 1 / sqrt(2)
				player->dy *= 0.70710678f;
			}

			if (app.keyboard[SDL_SCANCODE_Q])
			{
				app.keyboard[SDL_SCANCODE_Q] = 0;

				SDL_Color color = { rand() % 255, rand() % 255, rand() % 255, 255 };
				player->color = color;
			}

			if (app.keyboard[SDL_SCANCODE_LSHIFT] || app.keyboard[SDL_SCANCODE_RSHIFT])
			{
				player->dx *= PLAYER_SPEED_BOOST;
				player->dy *= PLAYER_SPEED_BOOST;
			}
			if (app.keyboard[SDL_SCANCODE_LCTRL] || app.keyboard[SDL_SCANCODE_RCTRL])
			{
				player->dx /= PLAYER_SPEED_BOOST;
				player->dy /= PLAYER_SPEED_BOOST;
			}

			// Test: local player reference
			if (app.keyboard[SDL_SCANCODE_E])
			{
				app.keyboard[SDL_SCANCODE_E] = 0;

				printf("Player (%3.f, %3.f)\n\n", game.localPlayer->x, game.localPlayer->y);
			}
		}

		player->x += player->dx;
		player->y += player->dy;

		if (player->x < 0)
			player->x = 0;
		if (player->x > DEFAULT_WINDOW_WIDTH - player->w)
			player->x = DEFAULT_WINDOW_WIDTH - player->w;
		if (player->y < 0)
			player->y = 0;
		if (player->y > DEFAULT_WINDOW_HEIGHT - player->h)
			player->y = DEFAULT_WINDOW_HEIGHT - player->h;

		if (player->health <= 0)
			destroyPlayer(player);
	}
}

static void drawPlayer(Entity* player)
{
	SDL_Rect playerRect = { 0 };
	char buffer[4];

	playerRect.x = player->x;
	playerRect.y = player->y;
	playerRect.w = player->w;
	playerRect.h = player->h;

	blitRect(playerRect.x, playerRect.y, playerRect.w, playerRect.h, player->color);

	// If it's the local player, draw a square around
	if (player == game.localPlayer)
	{
		SDL_Color colorBorder = { 255, 0, 0, 255 };
		blitRectBorder(playerRect.x - 5, playerRect.y -5, playerRect.w + 10, playerRect.h + 10, colorBorder);
	}

	// Text under player: P + ID
	secure_sprintf(buffer, sizeof(buffer), "P%d", player->id);
	drawTextScaled(player->x + (player->w / 2), player->y + 32, 0.6f, 0, 0, 0, TEXT_CENTER, buffer);
}

void drawPlayers(void)
{
	// Remote players first, so the local one is drawn on top
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (game.players[i].active && &game.players[i] != game.localPlayer)
			drawPlayer(&game.players[i]);
	}

	if (game.localPlayer != NULL)
		drawPlayer(game.localPlayer);
}
