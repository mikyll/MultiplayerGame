#include "menu.h"


void initMenu(void);
static void doMenu(float deltaTime);
static void doMenuWait(float deltaTime);
static void drawMenu(void);
static void drawMenuWait(void);


static Button buttonColor;
static Button buttonServer;
static Button buttonClient;

SDL_Color colorPlayer;

void initMenu(void)
{
  colorPlayer = getRandomColor();
  SDL_Color colorButtons = { 41, 170, 225, 255 };
  SDL_Color colorText = { 255, 255, 255 };
  SDL_Color colorTextDark = { 0, 0, 0 };

  buttonColor = createButton(
    DEFAULT_WINDOW_WIDTH * (1.0f / 3.0f),
    DEFAULT_WINDOW_HEIGHT * (1.0f / 4.0f),
    DEFAULT_WINDOW_WIDTH * (1.0f / 3.0f),
    DEFAULT_WINDOW_HEIGHT * (1.0f / 8.0f),
    colorPlayer,
    "-",
    isLight(colorPlayer) ? colorTextDark : colorText,
    NULL,
    NULL,
    NULL,
    NULL
  );
  updateButtonText(&buttonColor, "#%06X",
    hexFromRGB(colorPlayer.r, colorPlayer.g, colorPlayer.b));

  buttonServer = createButton(
    DEFAULT_WINDOW_WIDTH * (1.0f / 4.0f),
    DEFAULT_WINDOW_HEIGHT * (2.25f / 4.0f),
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
  buttonColor.c.a = !isMouseOverButton(app.mouse, buttonColor) * 55.0f + 200;
  buttonServer.c.a = !isMouseOverButton(app.mouse, buttonServer) * 55.0f + 200;
  buttonServer.c.b = !isMouseOverButton(app.mouse, buttonServer) * 30.0f + 225;
  buttonClient.c.a = !isMouseOverButton(app.mouse, buttonClient) * 55.0f + 200;
  buttonClient.c.b = !isMouseOverButton(app.mouse, buttonClient) * 30.0f + 225;

  if (app.mouse.button[SDL_BUTTON_LEFT])
  {
    app.mouse.button[SDL_BUTTON_LEFT] = 0;

    if (isMouseOverButton(app.mouse, buttonColor)) {
      colorPlayer = getRandomColor();
      SDL_Color colorText = { 255, 255, 255 };
      SDL_Color colorTextDark = { 0, 0, 0 };

      buttonColor.c = colorPlayer;
      buttonColor.textC = isLight(colorPlayer) ? colorTextDark : colorText;
    }

    if (isMouseOverButton(app.mouse, buttonServer) ||
      isMouseOverButton(app.mouse, buttonClient))
    {
      initGame();

      // Server
      if (isMouseOverButton(app.mouse, buttonServer))
      {
        setHostType(NET_HOST_SERVER);

        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Selected: SERVER");
      }
      // Client
      if (isMouseOverButton(app.mouse, buttonClient))
      {
        setHostType(NET_HOST_CLIENT);

        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Selected: CLIENT");

        // Wait for connection attempt (can fail)
        app.delegate.logic = doMenuWait;
        app.delegate.draw = drawMenuWait;
      }

      setAfterConnect(startGame, showMenu);
      setAfterDisconnect(showMenu);

      // Create the host
      if (netCreateHost(getTextInput(), DEFAULT_PORT) < 0)
      {
        // TEST
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "menu.c: Client creation failed");

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
  buttonColor.c.a = 255;
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
  drawButton(buttonColor);
  drawButton(buttonServer);
  drawButton(buttonClient);

  drawTextInput();
}

static void drawMenuWait(void)
{
  drawButton(buttonColor);
  drawButton(buttonServer);
  drawButton(buttonClient);
  drawTextScaled(DEFAULT_WINDOW_WIDTH / 2, DEFAULT_WINDOW_HEIGHT * (7.0f / 8.0f), 0.75, 0, 0, 0, TEXT_CENTER, "Connection attempt...");

  drawTextInput();
}

