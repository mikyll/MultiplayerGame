#include "main.h"


static void initApp(void);
static void cleanupApp(void);
static float calculateDeltaTime(void);
static void capFrameRate(Uint32 fpsCap);
static float calculateFPS(float deltaTime);


App app;


int main(int argc, char** argv)
{
    float deltaTime = 0.0f;
    float fps = 0.0f;

	initApp();

    showMenu();

	while (1)
	{
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

        capFrameRate(CAP_FPS);
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
    // High resolution counter: SDL_GetTicks only has millisecond precision
    static Uint64 lastFrameTime = 0;
    Uint64 now = SDL_GetPerformanceCounter();
    float deltaTime = 0.0f;

    if (lastFrameTime != 0)
        deltaTime = (float)(now - lastFrameTime) / SDL_GetPerformanceFrequency();
    lastFrameTime = now;

    // Clamp maximum delta time value
    if (deltaTime > 0.05f)
        deltaTime = 0.05f;

    return deltaTime;
}

static void capFrameRate(Uint32 fpsCap)
{
    // Absolute deadline of the current frame: advancing it by the exact frame duration lets
    // early/late wake-ups compensate each other, so the average matches fpsCap
    static Uint64 nextFrame = 0;
    Uint64 frequency = SDL_GetPerformanceFrequency();
    Uint64 frameDuration = frequency / fpsCap;
    Uint64 now = SDL_GetPerformanceCounter();

    if (nextFrame == 0)
        nextFrame = now;
    nextFrame += frameDuration;

    if (now < nextFrame)
        SDL_Delay((Uint32)((nextFrame - now) * 1000 / frequency));
    else if (now - nextFrame > frameDuration)
        // Too far behind (e.g. window dragged): restart from now instead of rushing frames to catch up
        nextFrame = now;
}

static float calculateFPS(float deltaTime)
{
    // Smoothing factor for the EWMA calculation
    const float smoothingFactor = 0.1f;

    // Initialize static variables to store previous FPS and the first frame flag
    static float smoothedFPS = 0.0f;
    static int isFirstFrame = 1;

    // A zero frame time would yield an infinite FPS that the average never recovers from
    if (deltaTime <= 0.0f)
        return smoothedFPS;

    // Calculate the reciprocal of frame time (FPS) for the current frame
    float instantFPS = 1.0f / deltaTime;

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
