#include "pch.h"

bool CheckCollision_Instances(Instance* i1, Instance* i2)
{
	// inspired by...
	//https://developer.mozilla.org/en-US/docs/Games/Techniques/2D_collision_detection


	return
		(
			i1->posX < i2->posX + i2->sprite->texW * i2->scaleX &&
			i1->posX + i1->sprite->texW * i1->scaleX > i2->posX &&
			i1->posY < i2->posY + i2->sprite->texH * i2->scaleY &&
			i1->posY + i1->sprite->texH * i1->scaleY > i2->posY
		);
}

void CheckCollision_Threads(Thread* t1, Thread* t2)
{
	// [placeholder]
	// If either thread has a flag that 
	// says "skip collision", then return

	// if there is no collision
	if (!CheckCollision_Instances(t1->inst, t2->inst))
	{
		// exit function
		return;
	}

	// execute collision functions, if they exist

	// execute function if it exists
	if (t1->funcOnCollide != 0)
	{
		// first pass "self", then pass "other"
		t1->funcOnCollide(t1, t2);
	}

	// execute function if it exists
	if (t2->funcOnCollide != 0)
	{
		// first pass "self", then pass "other"
		t2->funcOnCollide(t2, t1);
	}
}