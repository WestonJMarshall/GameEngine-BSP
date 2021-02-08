#pragma once
#include "pch.h"

// This is only intended to be here
// to cast several pool types to this one.
// This would be a great time for C++ and OOP
struct Item
{
	Item* prev;
	Item* next;
};

enum PoolTypes
{
	PL_THREAD,
	PL_INSTANCE,
	PL_NUM_TYPES
};

struct Pool
{
	Item* linkedList_FreeItems;
	Item* linkedList_TakenItems;

	int freeItems;
	int maxItems;
	int sizeOfEachItem;
	int sizeOfPool;

	// which is also the first item
	void* ptrPoolMemory;
};

void Pool_Init(Pool* pool, int maxItems, int sizeOfEachItem, const char* name);

void Pool_EraseAllItems(Pool* pool);

void Pool_EraseItem(Pool* pool, Item* item);

Item* Pool_NewItem(Pool* pool);