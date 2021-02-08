#pragma once
#include "pch.h"

// do all thread work
void Engine_HandleThreadUpdates();
void Engine_HandleThreadCollisions();
void Engine_HandleThreadDeaths();

// do all rendering work
void Engine_HandleRendering();