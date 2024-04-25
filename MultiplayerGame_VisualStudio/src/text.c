#include "text.h"

static SDL_Texture* fontTexture;
static char drawTextBuffer[MAX_LINE_LENGTH];

void initFonts(void)
{
	fontTexture = loadTexture("resources/fonts/font.png");
}

void drawText(int x, int y, int r, int g, int b, int align, char* format, ...)
{
	int i, len, c;
	SDL_Rect rect = { 0 };
	va_list args;

	memset(&drawTextBuffer, '\0', sizeof(drawTextBuffer));

	va_start(args, format);
	secure_sprintf(drawTextBuffer, sizeof(drawTextBuffer), format, args);
	va_end(args);

	len = strlen(drawTextBuffer);

	switch (align)
	{
        case TEXT_RIGHT:
        {
            x -= (len * GLYPH_WIDTH);

            break;
        }
        case TEXT_CENTER:
        {
            x -= (len * GLYPH_WIDTH) / 2;

            break;
        }
	}

	rect.w = GLYPH_WIDTH;
	rect.h = GLYPH_HEIGHT;
	rect.y = 0;

	SDL_SetTextureColorMod(fontTexture, r, g, b);

	for (i = 0; i < len; i++)
	{
		c = drawTextBuffer[i];

        if (c >= 'a' && c <= 'z')
        {
            c -= 32;
        }
		if (c >= ' ' && c <= '_')
		{
			rect.x = (c - ' ') * GLYPH_WIDTH;

			blitRectTexture(fontTexture, &rect, x, y);

			x += GLYPH_WIDTH;
		}
	}
}

void drawTextScaled(int x, int y, float size, int r, int g, int b, int align, char* format, ...)
{
    int i, len, c;
    SDL_Rect rect = { 0 };
    va_list args;

    memset(&drawTextBuffer, '\0', sizeof(drawTextBuffer));

    va_start(args, format);
    secure_sprintf(drawTextBuffer, sizeof(drawTextBuffer), format, args);
    va_end(args);

    len = strlen(drawTextBuffer);

    int scaledWidth = (int)(GLYPH_WIDTH * size);

    switch (align)
    {
        case TEXT_RIGHT:
        {
            x -= (len * scaledWidth);

            break;
        }
        case TEXT_CENTER:
        {
            x -= (len * scaledWidth) / 2;

            break;
        }
    }

    rect.w = GLYPH_WIDTH;
    rect.h = GLYPH_HEIGHT;
    rect.y = 0;

    SDL_SetTextureColorMod(fontTexture, r, g, b);

    for (i = 0; i < len; i++)
    {
        c = drawTextBuffer[i];

        if (c >= 'a' && c <= 'z')
        {
            c -= 32;
        }
        if (c >= ' ' && c <= '_')
        {
            rect.x = (c - ' ') * GLYPH_WIDTH;

            blitRectScaledTexture(fontTexture, &rect, x, y, size);

            x += scaledWidth;
        }
    }
}
