#pragma once
#include "pch.h"

enum SpriteTypes
{
	SP_PLAYER,
	SP_COIN,
	SP_NUM_TYPES
};

struct Sprite
{
	unsigned int texture;
	unsigned int startingVertex;

	// For collision
	unsigned short texW, texH;
};

Sprite* Sprite_Init(const char* filename);