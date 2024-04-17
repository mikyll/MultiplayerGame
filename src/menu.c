#include "menu.h"

typedef struct {
	int x, y;
	int w, h;
	SDL_Color c;
} Button;

static void initMenu();
static void logic();
static void draw();

static Button buttonServer;
static Button buttonClient;

static void initMenu()
{
	// Init button server
	buttonServer.x = DEFAULT_WINDOW_WIDTH * (1.0f / 4.0f);
	buttonServer.y = DEFAULT_WINDOW_HEIGHT * (2.0f / 4.0f);
	buttonServer.w = DEFAULT_WINDOW_WIDTH * (1.0f / 2.0f);
	buttonServer.h = DEFAULT_WINDOW_HEIGHT * (1.0f / 8.0f);
	SDL_Color colorServer = { 41, 170, 225 };
	buttonServer.c = colorServer;

	// Init button client
	buttonClient.x = DEFAULT_WINDOW_WIDTH * (1.0f / 4.0f);
	buttonClient.y = DEFAULT_WINDOW_HEIGHT * (3.0f / 4.0f);
	buttonClient.w = DEFAULT_WINDOW_WIDTH * (1.0f / 2.0f);
	buttonClient.h = DEFAULT_WINDOW_HEIGHT * (1.0f / 8.0f);
	SDL_Color colorClient = { 41, 170, 225 };
	buttonClient.c = colorClient;
}

void showMenu()
{
	initMenu();

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static int isMouseOverButton(Mouse mouse, Button button)
{
	return mouse.x >= button.x &&
		mouse.x <= button.x + button.w &&
		mouse.y >= button.y &&
		mouse.y <= button.y + button.h;
}

static void logic(float deltaTime)
{
	buttonServer.c.a = !isMouseOverButton(app.mouse, buttonServer) * 55.0f + 200;
	buttonServer.c.b = !isMouseOverButton(app.mouse, buttonServer) * 30.0f + 225;
	buttonClient.c.a = !isMouseOverButton(app.mouse, buttonClient) * 55.0f + 200;
	buttonClient.c.b = !isMouseOverButton(app.mouse, buttonClient) * 30.0f + 225;

	if (app.mouse.button[SDL_BUTTON_LEFT])
	{
		app.mouse.button[SDL_BUTTON_LEFT] = 0;

		if (isMouseOverButton(app.mouse, buttonServer) ||
			isMouseOverButton(app.mouse, buttonClient))
		{
			initGame();

			// Server
			if (isMouseOverButton(app.mouse, buttonServer))
			{
				printf("Server\n");
				setHostType(NET_HOST_SERVER);
			}
			// Client
			if (isMouseOverButton(app.mouse, buttonClient))
			{
				printf("Client\n");
				setHostType(NET_HOST_CLIENT);

				// TODO
				//setAddress("localhost");
			}

			// Create the host
			if (createHost() != 0)
			{
				disposeHost();
				return;
			}

			// Show game view
			showGame();
		}
	}

	doTextInput();
}

void draw()
{
	blitRect(buttonServer.x, buttonServer.y, buttonServer.w, buttonServer.h, buttonServer.c);
	blitRect(buttonClient.x, buttonClient.y, buttonClient.w, buttonClient.h, buttonClient.c);

	float verticalOffsetServer = buttonServer.y + buttonServer.h / 2 + (-GLYPH_HEIGHT / 2);
	drawText(DEFAULT_WINDOW_WIDTH / 2, verticalOffsetServer, 255, 255, 255, TEXT_CENTER, "Server");
	float verticalOffsetClient = buttonClient.y + buttonClient.h / 2 + (-GLYPH_HEIGHT / 2);
	drawText(DEFAULT_WINDOW_WIDTH / 2, verticalOffsetClient, 255, 255, 255, TEXT_CENTER, "Client");

	drawTextInput();
}