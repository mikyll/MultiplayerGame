#include "menu.h"

// RectangleButton
typedef struct Button Button;
struct Button {
	int x, y;
	int w, h;
	SDL_Color c;
	char text[MAX_BUTTON_TEXT_LENGTH];
	SDL_Color textC;
	void (*onOver)(void);
	void (*onOut)(void);
	void (*onPress)(void);
	void (*onRelease)(void);
	Button* next;
};

static void initMenu(void);
static void doMenu(float deltaTime);
static void doMenuWait(float deltaTime);
static void drawMenu(void);
static void drawMenuWait(void);

static Button buttonServer;
static Button buttonClient;

Button createButton(
	int posX, int posY, int width, int height, SDL_Color color, 
	char* text,	SDL_Color textColor,
	void (*onOver)(void), void (*onOut)(void),
	void (*onPress)(void), void (*onRelease)(void))
{
	Button button = { 0 };

	button.x = posX;
	button.y = posY;
	button.w = width;
	button.h = height;
	button.c = color;
	if (text != NULL && strlen(text) > 0)
	{
		secure_strcpy(button.text, sizeof(button.text), text);
		button.textC = textColor;
	}
	button.onOver = onOver;
	button.onOut = onOut;
	button.onPress = onPress;
	button.onRelease = onRelease;

	return button;
}

void drawButton(Button button)
{
	blitRect(button.x, button.y, button.w, button.h, button.c);
	drawText(
		button.x + button.w / 2, 
		button.y + button.h / 2 - GLYPH_HEIGHT / 2, 
		button.textC.r, 
		button.textC.g, 
		button.textC.b, 
		TEXT_CENTER, 
		button.text
	);
}

static void initMenu(void)
{
	SDL_Color colorButtons = { 41, 170, 225, 255 };
	SDL_Color colorText = { 255, 255, 255 };

	buttonServer = createButton(
		DEFAULT_WINDOW_WIDTH * (1.0f / 4.0f),
		DEFAULT_WINDOW_HEIGHT * (2.0f / 4.0f),
		DEFAULT_WINDOW_WIDTH * (1.0f / 2.0f),
		DEFAULT_WINDOW_HEIGHT * (1.0f / 8.0f),
		colorButtons,
		"Server",
		colorText,
		NULL,
		NULL,
		NULL,
		NULL
	);

	buttonClient = createButton(
		DEFAULT_WINDOW_WIDTH * (1.0f / 4.0f),
		DEFAULT_WINDOW_HEIGHT * (3.0f / 4.0f),
		DEFAULT_WINDOW_WIDTH * (1.0f / 2.0f),
		DEFAULT_WINDOW_HEIGHT * (1.0f / 8.0f),
		colorButtons,
		"Client",
		colorText,
		NULL,
		NULL,
		NULL,
		NULL
	);
}

void showMenu(void)
{
	initMenu();

	app.delegate.logic = doMenu;
	app.delegate.draw = drawMenu;
}

static int isMouseOverButton(Mouse mouse, Button button)
{
	return mouse.x >= button.x &&
		mouse.x <= button.x + button.w &&
		mouse.y >= button.y &&
		mouse.y <= button.y + button.h;
}

static void doMenu(float deltaTime)
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

				app.delegate.logic = doMenuWait;
				app.delegate.draw = drawMenuWait;
			}

			setAfterConnect(showGame, showMenu);
			setAfterDisconnect(showMenu);

			// Create the host
			if (netCreateHost(getTextInput(), PORT) < 0)
			{
				// TEST
				printf("menu.c: Client creation failed\n");

				// Creation failed
				netDisposeHost();

				app.delegate.logic = doMenu;
				app.delegate.draw = drawMenu;

				return;
			}
		}
	}

	doTextInput();
}

static void doMenuWait(float deltaTime)
{
	buttonServer.c.a = 255;
	buttonServer.c.b = 255;
	buttonClient.c.a = 255;
	buttonClient.c.b = 255;
	
	// Receive network input
	doNetworkingBefore();

	// Send network update
	doNetworkingAfter();
}

static void drawMenu(void)
{
	drawButton(buttonServer);
	drawButton(buttonClient);
	
	drawTextInput();
}

static void drawMenuWait(void)
{
	drawButton(buttonServer);
	drawButton(buttonClient);
	drawTextScaled(DEFAULT_WINDOW_WIDTH / 2, DEFAULT_WINDOW_HEIGHT * (7.0f / 8.0f), 0.75, 0, 0, 0, TEXT_CENTER, "Connection attempt...");

	drawTextInput();
}
