#include "pch.h"

void Pool_Init(Pool* pool, int maxItems, int sizeOfEachItem, const char* name)
{
	memset(pool, 0, sizeof(Pool));

	pool->maxItems = maxItems;
	pool->sizeOfEachItem = sizeOfEachItem;
	pool->sizeOfPool = maxItems * sizeOfEachItem;

	void* poolMem = (void*)MEMPACK_AllocMem(&global.ramPack, maxItems * sizeOfEachItem, name);

	// which is also the first item
	pool->ptrPoolMemory = poolMem;

	Pool_EraseAllItems(pool);
}

void Pool_EraseAllItems(Pool* pool)
{
	// must be char pointer so we can increment
	// the pointer by a number of bytes

	char* itemAlloc = (char*)pool->ptrPoolMemory;

	pool->freeItems = 0;
	pool->linkedList_FreeItems = 0;
	pool->linkedList_TakenItems = 0;

	for (int i = 0; i < pool->maxItems; i++)
	{
		Item* item = (Item*)itemAlloc;
		item->prev = (Item*)0;
		item->next = (Item*)0;
		Pool_EraseItem(pool, item);

		itemAlloc += pool->sizeOfEachItem;
	}
}

void Pool_EraseItem(Pool* pool, Item* item)
{
	// check for nullptr
	if (item == 0)
		return;

	// make backup
	Item* itemPrev = item->prev;
	Item* itemNext = item->next;

	// if this is the first free item added,
	// which is the end of the linked list
	if (pool->linkedList_FreeItems == 0)
	{
		// initialize to zero
		item->next = 0;

		// otherwise, dont worry, we'll overwrite anyways
	}

	// if this is not the first member added,
	// which is not the end of the list
	else
	{
		// link with existing members
		item->next = pool->linkedList_FreeItems;
		pool->linkedList_FreeItems->prev = item;
	}


	// Put this item inside the linked list of free items
	item->prev = (Item*)0;
	pool->linkedList_FreeItems = item;

	// if this is the most recent item added to taken
	if (item == pool->linkedList_TakenItems)
	{
		// set linked list of taken to what item->next USED to be,
		// before it was changed to freeItems
		pool->linkedList_TakenItems = itemNext;

		// if the new item exists
		if (pool->linkedList_TakenItems != nullptr)
		{
			// no link
			pool->linkedList_TakenItems->prev = 0;
		}
	}

	// If this was not the most recent item added to taken
	else
	{
		// link oldPrev->next to oldNext
		if (itemPrev != nullptr)
		{
			itemPrev->next = itemNext;
		}

		// link oldNext->prev to oldPrev
		if (itemNext != nullptr)
		{
			itemNext->prev = itemPrev;
		}
	}

	pool->freeItems++;
}

Item* Pool_NewItem(Pool* pool)
{
	if (pool->freeItems == 0)
	{
		printf("ERROR: No free items left in pool\n");
		while (true) {}
	}

	// get the first free item
	Item* item = pool->linkedList_FreeItems;

	// shift the free items list
	pool->linkedList_FreeItems = pool->linkedList_FreeItems->next;
	pool->linkedList_FreeItems->prev = 0;

	// if this is the first item allocated
	if (pool->linkedList_TakenItems == 0)
	{
		// set to nullptr so it doesn't link
		// into the free items
		item->next = 0;
	}

	// if this is not the first item allocated
	else
	{
		// link with existing members
		item->next = pool->linkedList_TakenItems;
		pool->linkedList_TakenItems->prev = item;
	}

	// add to list of taken
	item->prev = (Item*)0;
	pool->linkedList_TakenItems = item;

	pool->freeItems--;

	return item;
}