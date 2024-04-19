#pragma once

#include "../common.h"

// Text
extern void drawText(int x, int y, int r, int g, int b, int align, char* format, ...);
extern void drawTextScaled(int x, int y, float size, int r, int g, int b, int align, char* format, ...);

// Utils
extern int secure_sprintf(char* buffer, size_t bufferSize, const char* format, ...);

extern App app;