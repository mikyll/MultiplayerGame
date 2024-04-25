#include "main.h"


static void initApp(void);
static void cleanupApp(void);
static float calculateDeltaTime(void);
static void capFrameRate(Uint32 initFrameTime, Uint32 fpsCap);
static float calculateFPS(float deltaTime);


App app;


int main(int argc, char** argv)
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

static void initApp(void)
{
    memset(&app, 0, sizeof(App));
    app.textureTail = &app.textureHead;

    srand(time(NULL));

    initSDL();
    initFonts();
    initEnet();

    atexit(cleanupApp);
}

static void cleanupApp(void)
{
    cleanupSDL();

    cleanupEnet();
}

static float calculateDeltaTime(void)
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
    // Smoothing factor for the EWMA calculation
    const float smoothingFactor = 0.1f;

    // Calculate the reciprocal of frame time (FPS) for the current frame
    float instantFPS = 1.0f / deltaTime;

    // Initialize static variables to store previous FPS and the first frame flag
    static float smoothedFPS = 0.0f;
    static int isFirstFrame = 1;

    // If it's the first frame, set smoothed FPS to the instant FPS
    if (isFirstFrame)
    {
        smoothedFPS = instantFPS;
        isFirstFrame = 0;
    }
    else
    {
        // Otherwise, update smoothed FPS using EWMA
        smoothedFPS = smoothingFactor * instantFPS + (1.0f - smoothingFactor) * smoothedFPS;
    }

    return smoothedFPS;
}
