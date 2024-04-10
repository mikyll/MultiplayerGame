#pragma once

#include "common.h"

void initGame();

// Player
extern int createPlayer(float x, float y);
extern void doPlayers();
extern void drawPlayers();

extern App app;