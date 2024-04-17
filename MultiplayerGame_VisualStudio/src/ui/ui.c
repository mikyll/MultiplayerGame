#include "ui.h"

#define IP_MAX_LENGTH	16

char ipAddress[IP_MAX_LENGTH] = "";
int cursor = 0;

void doCharacter(int scanCode)
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

void doTextInput()
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

void drawTextInput()
{
	drawText(DEFAULT_WINDOW_WIDTH / 2 - GLYPH_WIDTH * 12, GLYPH_HEIGHT, 0, 0, 0, TEXT_LEFT, "IP ADDRESS: ");
	if (cursor == 0)
	{
		drawText(DEFAULT_WINDOW_WIDTH / 2, GLYPH_HEIGHT, 0, 0, 0, TEXT_LEFT, "localhost");
	}
	else
	{
		drawText(DEFAULT_WINDOW_WIDTH / 2, GLYPH_HEIGHT, 0, 0, 0, TEXT_LEFT, ipAddress);
	}
}

void logic()
{
	
}

void drawUI()
{

}