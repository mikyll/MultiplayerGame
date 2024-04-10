#include "player.h"

int myID = 1;

int createPlayer(float x, float y)
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

	return game.playersCount;
}

void doPlayers()
{
	Entity* player, * playerPrev;

	playerPrev = &game.playersHead;
	for (player = game.playersHead.next; player != NULL; player = player->next)
	{
		player->dx = 0;
		player->dy = 0;

		if (player->id == myID)
		{
			if (app.keyboard[SDL_SCANCODE_W])
				player->dy = -PLAYER_SPEED;
			if (app.keyboard[SDL_SCANCODE_S])
				player->dy = PLAYER_SPEED;
			if (app.keyboard[SDL_SCANCODE_A])
				player->dx = -PLAYER_SPEED;
			if (app.keyboard[SDL_SCANCODE_D])
				player->dx = PLAYER_SPEED;

			if (app.keyboard[SDL_SCANCODE_LSHIFT])
			{
				player->dx *= PLAYER_SPEED_BOOST;
				player->dy *= PLAYER_SPEED_BOOST;
			}
		}

		player->x += player->dx;
		player->y += player->dy;

		if (player->x < 0)
			player->x = 0;
		if (player->x > DEFAULT_SCREEN_WIDTH - player->w)
			player->x = DEFAULT_SCREEN_WIDTH - player->w;
		if (player->y < 0)
			player->y = 0;
		if (player->y > DEFAULT_SCREEN_HEIGHT - player->h)
			player->y = DEFAULT_SCREEN_HEIGHT - player->h;
	}
}

void drawPlayers()
{
	Entity* player;
	for (player = game.playersHead.next; player != NULL; player = player->next)
	{
		SDL_Rect playerRect;
		playerRect.x = (int)player->x;
		playerRect.y = (int)player->y;
		playerRect.w = (int)player->w;
		playerRect.h = (int)player->h;

		blitRect(playerRect.x, playerRect.y, playerRect.w, playerRect.h, player->color);

		if (player->id == myID)
		{
			SDL_Color colorBorder = { 255, 0, 0, 255 };
			blitRectBorder(playerRect.x - 5, playerRect.y -5, playerRect.w + 10, playerRect.h + 10, colorBorder);
		}
	}
}