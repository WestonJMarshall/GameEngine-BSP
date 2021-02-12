#include "pch.h"

bool Bounding_Box_Collision(BoundingBox& b1, BoundingBox& b2)
{
	if (b1.left > b2.right
		|| b1.right < b2.left
		|| b1.front < b2.back
		|| b1.back > b2.front
		|| b1.top < b2.bottom
		|| b1.bottom > b2.top)
	{
		return false;
	}
	return true;
}

bool Bounding_Box_Collision_2D(BoundingBox& b1, BoundingBox& b2)
{
	if (b1.left > b2.right
		|| b1.right < b2.left
		|| b1.top < b2.bottom
		|| b1.bottom > b2.top)
	{
		return false;
	}
	return true;
}