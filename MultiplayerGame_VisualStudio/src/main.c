#include "main.h"

static void initApp();
static void cleanupApp();
static float calculateDeltaTime();
static void capFrameRate(Uint32 initFrameTime, Uint32 fpsCap);
static float calculateFPS(float deltaTime);

int main()
{
    Uint32 initFrameTime = 0.0f;
    float deltaTime = 0.0f;
    float fps = 0.0f;

	initApp();

    showMenu();

	while (1)
	{
        initFrameTime = SDL_GetTicks();
        deltaTime = calculateDeltaTime();

		prepareScene();

        // Input
		doInput();

		// Logic
		app.delegate.logic(deltaTime);

		// Draw
		app.delegate.draw();
        drawFPS(fps);

		presentScene();

        fps = calculateFPS(deltaTime);

        capFrameRate(initFrameTime, CAP_FPS);
	}

	return 0;
}

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

static float calculateDeltaTime()
{
    static Uint32 lastFrameTime = 0.0f;
    float deltaTime;

    deltaTime = (float)(SDL_GetTicks() - lastFrameTime) / 1000.0f;
    lastFrameTime = SDL_GetTicks();

    // Clamp maximum delta time value
    if (deltaTime > 0.05f)
        deltaTime = 0.05f;

    return deltaTime;
}

static void capFrameRate(Uint32 initFrameTime, Uint32 fpsCap)
{
    Uint32 frameTime = SDL_GetTicks() - initFrameTime;
    Uint32 frameDurationMS = 1000.0f / fpsCap;

    if (frameTime < frameDurationMS)
        SDL_Delay(frameDurationMS - frameTime);
}

static float calculateFPS(float deltaTime)
{
    static float frameTimes[100] = { 0 };
    static int frameIndex = 0;
    static float fps = 0.0f;

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
    return fps;
}