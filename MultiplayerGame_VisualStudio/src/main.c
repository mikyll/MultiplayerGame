#include "main.h"

static void initApp();
static void cleanupApp();

static void initApp()
{
	memset(&app, 0, sizeof(App));
    app.textureTail = &app.textureHead;

	srand(time(NULL));

	initSDL();
    initFonts();
    initEnet();

	atexit(cleanupApp);
}

static void cleanupApp()
{
    cleanupSDL();

    cleanupEnet();
}

Uint32 lastFrameTime = 0;
int frames = 0;
float fpsTimer = 0.0f;

static float doDeltaTime()
{
    float deltaTime;

    //while (!SDL_TICKS_PASSED(SDL_GetTicks(), lastFrameTime + 16));

    deltaTime = (float)(SDL_GetTicks() - lastFrameTime) / 1000.0f;
    lastFrameTime = SDL_GetTicks();

    // Clamp maximum delta time value
    if (deltaTime > 0.05f)
        deltaTime = 0.05f;

    return deltaTime;
}

static int doFPS(float deltaTime)
{
    int fps;

    frames++;
    fpsTimer += deltaTime;
    if (fpsTimer >= 1.0f) {
        fps = frames;
        frames = 0;
        fpsTimer -= 1.0f;
    }
}


static void capFrameRate(Uint32 currentTime, Uint32 fpsCap)
{
    Uint32 frameTime = SDL_GetTicks() - currentTime;
    Uint32 frameDurationMS = 1000.0f / fpsCap;
    if (frameTime < frameDurationMS) {
        SDL_Delay(frameDurationMS - frameTime);
    }
}

void drawFPS(int fps)
{
    char buffer[10];
    sprintf_s(buffer, sizeof(buffer), "%5d FPS", fps);
    drawTextScaled(DEFAULT_WINDOW_WIDTH - 5, 5, 0.75f, 0, 0, 0, TEXT_RIGHT, buffer);

}

float frameTimes[100]; // Array to store frame times
int frameIndex = 0;

int main()
{
    float currentTime = 0.0f;
    float deltaTime = 0.0f;
    float fps = 0.0f;

	initApp();

    showMenu();

	while (1)
	{
        currentTime = SDL_GetTicks();
        deltaTime = doDeltaTime();

		prepareScene();

        // Input
		doInput();

		// Logic
		app.delegate.logic(deltaTime);

		// Draw
		app.delegate.draw();
        drawFPS(fps);

		presentScene();

        frameTimes[frameIndex] = deltaTime;
        frameIndex = (frameIndex + 1) % 100; // Wrap around to the beginning if we reach the end of the array

        float averageFrameTime = 0.0f;
        for (int i = 0; i < 100; ++i) {
            averageFrameTime += frameTimes[i];
        }
        averageFrameTime /= 100.0f;

        if (averageFrameTime > 0.0f) {
            fps = 1.0f / averageFrameTime;
        }

        capFrameRate(currentTime, 60);
	}

	return 0;
}