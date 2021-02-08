#include "pch.h"

Thread* Thread_Init(int type, void (*onTick)(Thread* th))
{
	bool firstInBucket = false;
	if (global.threadBucket[type] == 0) firstInBucket = true;

	Thread* t = (Thread*)Pool_NewItem(&global.pool[PL_THREAD]);

	t->flags = 0;

	// give function pointer
	t->funcOnTick = onTick;
	t->funcOnCollide = 0;
	t->funcOnDestroy = 0;

	// default values are null
	t->parentThread = 0;
	t->siblingThread = 0;
	t->childThread = 0;

	// if linked list is nullptr
	if (global.threadBucket[type] == 0)
	{
		// set the first pointer of linked list
		global.threadBucket[type] = t;
	}

	// if the list exists
	else
	{
		// set current thread's sibling to the first member
		t->siblingThread = global.threadBucket[type];

		// make this thread the new first member
		global.threadBucket[type] = t;
	}

	return t;
}