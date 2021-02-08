#pragma once
#include "pch.h"

enum ThreadTypes
{
	TH_PLAYER,
	TH_COIN,
	TH_NUM_TYPES
};

enum ThreadFlags
{
	TH_FLAG_DEAD = 0x800,
	TH_FLAG_NO_COLLIDE = 0x1000,
};

struct Thread
{
	// used for Pool
	Thread* prev;
	Thread* next;

	int flags;

	Thread* parentThread;
	Thread* siblingThread;
	Thread* childThread;

	void (*funcOnDestroy)();
	void (*funcOnCollide)(Thread* self, Thread* other);
	void (*funcOnTick)(Thread* self);

	// pointer to any data used by the thread
	void* object;

	// Thread and Instance are linked together
	Instance* inst;
};

Thread* Thread_Init(int type, void (*onTick)(Thread* th));