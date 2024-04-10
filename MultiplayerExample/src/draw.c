#include "draw.h"

void prepareScene()
{
	SDL_SetRenderDrawColor(app.renderer, 255, 255, 255, 255);
	SDL_RenderClear(app.renderer);
}

void presentScene()
{
	SDL_RenderPresent(app.renderer);
}

/*static SDL_Texture* getTexture(char* name)
{
	Texture* t;
	
	for (t = app.textureHead.next ; t != NULL ; t = t->next)
	{
		if (strcmp(t->name, name) == 0)
		{
			return t->texture;
		}
	}
	
	return NULL;
}

static void addTextureToCache(char *name, SDL_Texture *sdlTexture)
{
	Texture* texture;
	
	texture = malloc(sizeof(Texture));
	memset(texture, 0, sizeof(Texture));
	app.textureTail->next = texture;
	app.textureTail = texture;
	
	STRNCPY(texture->name, name, MAX_NAME_LENGTH);
	texture->texture = sdlTexture;
}

SDL_Texture* loadTexture(char* filename)
{
	SDL_Texture* texture;
	
	texture = getTexture(filename);
	
	if (texture == NULL)
	{
		texture = IMG_LoadTexture(app.renderer, filename);
		if (!texture)
		{
			SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Loading of texture '%s' failed", filename);
			exit(1);
		}
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loaded texture '%s'", filename);
		addTextureToCache(filename, texture);
	}

	return texture;
}*/

void blit(SDL_Texture* texture, int x, int y, int center)
{
	SDL_Rect dest;
	
	dest.x = x;
	dest.y = y;
	SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
	
	if (center)
	{
		dest.x -= dest.w / 2;
		dest.y -= dest.h / 2;
	}
	
	SDL_RenderCopy(app.renderer, texture, NULL, &dest);
}

void blitRotated(SDL_Texture* texture, int x, int y, int center, float angle, SDL_RendererFlip flip)
{
	SDL_Rect dest;

	dest.x = x;
	dest.y = y;
	SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

	if (center)
	{
		dest.x -= (dest.w / 2);
		dest.y -= (dest.h / 2);
	}

	SDL_RenderCopyEx(app.renderer, texture, NULL, &dest, angle, NULL, flip);
}

void blitScaled(SDL_Texture* texture, int x, int y, int w, int h)
{
	SDL_Rect dest;

	dest.x = x;
	dest.y = y;
	dest.w = w;
	dest.h = h;

	SDL_RenderCopy(app.renderer, texture, NULL, &dest);
}

void blitRect(int x, int y, int w, int h, SDL_Color c)
{
	SDL_Rect rectToDraw = { x, y, w, h };
	SDL_Color prev;

	SDL_GetRenderDrawColor(app.renderer, &prev.r, &prev.g, &prev.b, &prev.a);
	SDL_SetRenderDrawColor(app.renderer, c.r, c.g, c.b, c.a);
	SDL_RenderFillRect(app.renderer, &rectToDraw);
	SDL_SetRenderDrawColor(app.renderer, prev.r, prev.g, prev.b, prev.a);
}

void blitRectBorder(int x, int y, int w, int h, SDL_Color c)
{
	SDL_Rect rectToDraw = { x, y, w, h };
	SDL_Color prev;

	SDL_GetRenderDrawColor(app.renderer, &prev.r, &prev.g, &prev.b, &prev.a);
	SDL_SetRenderDrawColor(app.renderer, c.r, c.g, c.b, c.a);
	SDL_RenderDrawRect(app.renderer, &rectToDraw);
	SDL_SetRenderDrawColor(app.renderer, prev.r, prev.g, prev.b, prev.a);
}

void blitRectTexture(SDL_Texture* texture, SDL_Rect* src, int x, int y)
{
	SDL_Rect dest;

	dest.x = x;
	dest.y = y;
	dest.w = src->w;
	dest.h = src->h;

	SDL_RenderCopy(app.renderer, texture, src, &dest);
}

void blitRectRotated(SDL_Texture* texture, SDL_Rect* src, int x, int y, int center, float angle, SDL_RendererFlip flip)
{
	SDL_Rect dest;

	dest.x = x;
	dest.y = y;
	dest.w = src->w;
	dest.h = src->h;

	if (center)
	{
		dest.x -= (dest.w / 2);
		dest.y -= (dest.h / 2);
	}

	SDL_RenderCopyEx(app.renderer, texture, src, &dest, angle, NULL, flip);
}