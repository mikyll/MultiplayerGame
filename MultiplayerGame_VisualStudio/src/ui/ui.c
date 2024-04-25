#include "ui.h"

#define IP_MAX_LENGTH	16


static void doCharacter(int scanCode);


static char ipAddress[IP_MAX_LENGTH] = "";
static int cursor = 0;


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
	for (int iScanCode = SDL_SCANCODE_1; iScanCode <= SDL_SCANCODE_PERIOD; iScanCode++)
	{
		if (app.keyboard[iScanCode])
		{
			app.keyboard[iScanCode] = 0;

			doCharacter(iScanCode);
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
	char buffer[10];
	secure_sprintf(buffer, sizeof(buffer), "%5d FPS", fps);
	drawTextScaled(DEFAULT_WINDOW_WIDTH - 5, 5, 0.75f, 0, 0, 0, TEXT_RIGHT, buffer);
}

void doUI(float deltaTime)
{
	
}

void drawUI(void)
{

}

