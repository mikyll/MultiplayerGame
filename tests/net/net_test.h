#pragma once

/*
* Shared helpers of the headless network test programs.
*/

#include <stdlib.h>
#include <unistd.h>


// Tests use their own port, so they don't clash with a running game
static int testPort(void)
{
    const char* port = getenv("MG_TEST_PORT");

    return port != NULL ? atoi(port) : 18987;
}

// The game code links drawing functions that headless tests never call
#define HEADLESS_DRAWING_STUBS \
    void blitRect(int x, int y, int w, int h, SDL_Color c) {} \
    void blitRectBorder(int x, int y, int w, int h, SDL_Color c) {} \
    void drawTextScaled(int x, int y, float size, int r, int g, int b, int align, char* format, ...) {}
