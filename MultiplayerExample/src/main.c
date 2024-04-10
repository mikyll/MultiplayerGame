#include "main.h"

static void initApp()
{
	memset(&app, 0, sizeof(App));

	srand(time(NULL));

	initSDL();
	atexit(cleanup);
}

int main()
{
	initApp();

	initGame();

	while (1)
	{
		prepareScene();

		// Input
		doInput();

		// Logic
		app.delegate.logic();

		// Draw
		app.delegate.draw();

		presentScene();

		SDL_Delay(16);
	}

	return 0;
}
