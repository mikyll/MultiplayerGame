#include "ui.h"

#define IP_MAX_LENGTH	16


static void doCharacter(int scanCode);


static char ipAddress[IP_MAX_LENGTH] = "";
static int cursor = 0;

// Keys handled by doCharacter: only these are consumed, so the others (e.g. RETURN) stay available
static const int textInputKeys[] = {
	SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4, SDL_SCANCODE_5,
	SDL_SCANCODE_6, SDL_SCANCODE_7, SDL_SCANCODE_8, SDL_SCANCODE_9, SDL_SCANCODE_0,
	SDL_SCANCODE_PERIOD, SDL_SCANCODE_BACKSPACE
};


static void doCharacter(int scanCode)
{
	if (scanCode == SDL_SCANCODE_BACKSPACE && cursor > 0)
	{
		cursor--;
		ipAddress[cursor] = '\0';
	}

	if (cursor >= IP_MAX_LENGTH - 1)
		return;

	if (scanCode >= SDL_SCANCODE_1 && scanCode <= SDL_SCANCODE_9)
	{
		ipAddress[cursor] = scanCode + 19;
		cursor++;
		ipAddress[cursor] = '\0';
	}
	if (scanCode == SDL_SCANCODE_0)
	{
		ipAddress[cursor] = '0';
		cursor++;
		ipAddress[cursor] = '\0';
	}
	if (scanCode == SDL_SCANCODE_PERIOD)
	{
		ipAddress[cursor] = '.';
		cursor++;
		ipAddress[cursor] = '\0';
	}
}

void doTextInput(void)
{
	for (size_t i = 0; i < sizeof(textInputKeys) / sizeof(textInputKeys[0]); i++)
	{
		if (app.keyboard[textInputKeys[i]])
		{
			app.keyboard[textInputKeys[i]] = 0;

			doCharacter(textInputKeys[i]);
		}
	}

	if (app.keyboard[SDL_SCANCODE_RETURN] && cursor >= 8)
	{
		app.keyboard[SDL_SCANCODE_RETURN] = 0;

		// confirm
		printf("%s\n", ipAddress);
	}
}

char* getTextInput(void)
{
	return ipAddress;
}

void drawTextInput(void)
{
	drawTextScaled(DEFAULT_WINDOW_WIDTH / 2 - GLYPH_WIDTH * 0.75f * 12, GLYPH_HEIGHT * 0.75f, 0.75f, 0, 0, 0, TEXT_LEFT, "IP ADDRESS: ");
	if (cursor == 0)
	{
		drawTextScaled(DEFAULT_WINDOW_WIDTH / 2, GLYPH_HEIGHT * 0.75f, 0.75f, 100, 100, 100, TEXT_LEFT, "localhost");
	}
	else
	{
		drawTextScaled(DEFAULT_WINDOW_WIDTH / 2, GLYPH_HEIGHT * 0.75f, 0.75f, 0, 0, 0, TEXT_LEFT, ipAddress);
	}
}

void drawFPS(int fps)
{
	// Large enough for any int, initialized in case formatting fails
	char buffer[16] = "";
	secure_sprintf(buffer, sizeof(buffer), "%5d FPS", fps);
	drawTextScaled(DEFAULT_WINDOW_WIDTH - 5, 5, 0.75f, 0, 0, 0, TEXT_RIGHT, buffer);
}

