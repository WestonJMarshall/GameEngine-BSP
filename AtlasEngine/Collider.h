#pragma once
#include "pch.h"

struct BoundingBox
{
	float left;
	float right;
	float top;
	float bottom;
	float front;
	float back;
};

bool Bounding_Box_Collision(BoundingBox& b1, BoundingBox& b2);

bool Bounding_Box_Collision_2D(BoundingBox& b1, BoundingBox& b2);

