#include "player.h"

int spawnPlayerAt(float x, float y);
void destroyPlayer(Entity* player);
void setLocalPlayer(Entity* player);
Entity* getPlayerByID(int id);


static Entity* localPlayer = NULL;

/*
* Creates a new Player entity, adding it to the players linked list.
* Returns the player's ID.
* 
* Useful to allocate an entity received by the network.
*/
int spawnPlayer(Entity p)
{
	// Allocate memory for the player entity
	Entity* player = (Entity*) malloc(sizeof(Entity));
	if (player == NULL)
	{
		printf("Player creation failed.\n");
		return;
	}

	// Increase player count
	game.playersCount++;

	// Init player parameters
	memset(player, 0, sizeof(Entity));
	player->id = p.id;
	player->w = p.w;
	player->h = p.h;
	player->x = p.x;
	player->y = p.y;
	player->color = p.color;
	player->next = NULL;

	// Add player to list
	game.playersTail->next = player;
	game.playersTail = player;

	return p.id;
}

int spawnPlayerAt(float x, float y)
{
	// Allocate memory for the player entity
	Entity* player = (Entity*)malloc(sizeof(Entity));
	if (player == NULL)
	{
		printf("Player creation failed.\n");
		return;
	}

	// Increase player count
	game.playersCount++;

	// Init player parameters
	memset(player, 0, sizeof(Entity));
	player->id = game.playersCount;
	player->w = PLAYER_SIZE;
	player->h = PLAYER_SIZE;
	player->x = x;
	player->y = y;
	player->dx = 0.0f;
	player->dy = 0.0f;
	SDL_Color color = { rand() % 255, rand() % 255, rand() % 255, 255 };
	player->color = color;
	player->next = NULL;

	// Add player to list
	game.playersTail->next = player;
	game.playersTail = player;

	return player->id;
}

void destroyPlayer(Entity* player)
{
	Entity* p, * playerPrev = NULL;

	playerPrev = &game.playersHead;
	for (p = game.playersHead.next; p != NULL; p = p->next)
	{
		if (p == player)
		{
			if (p == game.playersTail)
			{
				game.playersTail = playerPrev;
			}

			playerPrev->next = p->next;
			free(p);

			game.playersCount--;
			break;
		}
		playerPrev = p;
	}
}

void destroyPlayerByID(int id)
{
	Entity* p, * playerPrev = NULL;

	playerPrev = &game.playersHead;
	for (p = game.playersHead.next; p != NULL; p = p->next)
	{
		if (p->id == id)
		{
			printf("Player.c: destroy %d\n", id);
			if (p == game.playersTail)
			{
				game.playersTail = playerPrev;
			}

			playerPrev->next = p->next;
			free(p);

			game.playersCount--;
			break;
		}
		playerPrev = p;
	}
}

void setLocalPlayer(Entity* player)
{
	localPlayer = player;
}

Entity* getLocalPlayer()
{
	return localPlayer;
}

Entity* getPlayerByID(int id)
{
	Entity* player;
	for (player = game.playersHead.next; player != NULL; player = player->next)
	{
		if (player->id == id)
		{
			return player;
		}
	}
	return NULL;
}

static int isPlayerLast(Entity* player)
{
	Entity* p;

	for (p = game.playersHead.next; p != NULL; p = p->next) 
	{
		// If the player's ID matches myID and it's the last node, return 1
		if (p == player && p->next == NULL)
		{
			return 1; // Player is the last node
		}
	}

	// Player is not the last node
	return 0;
}

static void setPlayerToLast(Entity* player)
{
	if (player == NULL || isPlayerLast(player))
		return;

	Entity* p, * playerPrev = NULL, * playerLast = NULL;

	for (p = game.playersHead.next; p != NULL; playerPrev = p, p = p->next)
	{
		if (p == player)
		{
			playerLast = p;

			// Remove player from current position in the list
			if (playerPrev != NULL)
			{
				playerPrev->next = p->next;
			}
			else
			{
				game.playersHead.next = p->next;
			}
			break;
		}
		playerPrev = p;
	}

	// Append the player to the end of the list
	if (playerLast != NULL) {
		playerLast->next = NULL;
		game.playersTail->next = playerLast;
		game.playersTail = playerLast;
	}
}

void doPlayers(float deltaTime)
{
	setPlayerToLast(localPlayer);

	Entity* player, * playerPrev;

	playerPrev = &game.playersHead;
	for (player = game.playersHead.next; player != NULL; player = player->next)
	{
		player->dx = 0;
		player->dy = 0;

		if (player->id == localPlayer->id)
		{
			if (app.keyboard[SDL_SCANCODE_W])
				player->dy = -PLAYER_SPEED * deltaTime;
			if (app.keyboard[SDL_SCANCODE_S])
				player->dy = PLAYER_SPEED * deltaTime;
			if (app.keyboard[SDL_SCANCODE_A])
				player->dx = -PLAYER_SPEED * deltaTime;
			if (app.keyboard[SDL_SCANCODE_D])
				player->dx = PLAYER_SPEED * deltaTime;

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

				Entity* player;
				for (player = game.playersHead.next; player != NULL; player = player->next)
				{
					if (player == localPlayer)
					{
						printf("Player (%3.f, %3.f)\n", player->x, player->y);
						break;
					}
				}
				printf("Player (%3.f, %3.f)\n\n", localPlayer->x, localPlayer->y);
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

		playerPrev = player;
	}
}

void drawPlayers()
{
	Entity* player;
	SDL_Rect playerRect;
	char buffer[4];

	for (player = game.playersHead.next; player != NULL; player = player->next)
	{
		playerRect.x = player->x;
		playerRect.y = player->y;
		playerRect.w = player->w;
		playerRect.h = player->h;

		blitRect(playerRect.x, playerRect.y, playerRect.w, playerRect.h, player->color);

		// If it's the local player, draw a square around
		if (player == localPlayer)
		{
			SDL_Color colorBorder = { 255, 0, 0, 255 };
			blitRectBorder(playerRect.x - 5, playerRect.y -5, playerRect.w + 10, playerRect.h + 10, colorBorder);
		}

		// Text under player: P + ID
		secure_sprintf(buffer, sizeof(buffer), "P%d", player->id);
		drawTextScaled(player->x + (player->w / 2), player->y + 32, 0.6f, 0, 0, 0, TEXT_CENTER, buffer);
	}
}