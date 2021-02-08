#include "pch.h"

void Coin_OnCollide(Thread* self, Thread* other)
{
	// This thread will die after exection,
	// and the coin will disappear
	self->flags |= TH_FLAG_DEAD;

	// in the future, we will use "other" to 
	// add points to the player for getting coin
}