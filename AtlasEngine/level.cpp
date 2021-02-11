#include "pch.h"

void Level_Init()
{
    // Load all sprites
    global.sprite[SP_PLAYER] = Sprite_Init("Assets/man.bmp");
    global.sprite[SP_COIN] = Sprite_Init("Assets/coin.bmp");

    // Rather than allocating directly to the stack allocator, we make a buffer
    // with slots, so we can add and remove things as we need, without needing bookmarks.
    // Then, the stack allocator can continue allocating while we add/remove members
    Pool_Init(&global.pool[PL_THREAD], 10, sizeof(Thread), "ThreadPool");
    Pool_Init(&global.pool[PL_INSTANCE], 10, sizeof(Instance), "InstancePool");

    //The level's BSP pool
    Pool_Init(&global.pool[PL_BSP], 10, sizeof(BSP), "BSPPool");

    // Initialize Instance in Instance Pool
    Instance* in = (Instance*)Pool_NewItem(&global.pool[PL_INSTANCE]);

    // Initialize Thread, this Thread_Init pulls from Thread Pool
    Thread* th = Thread_Init(TH_PLAYER, Player_OnTick);

    // link instance and thread
    th->inst = in;
    in->thread = th;

    // draw person on top (small depth)
    in->depth = 0.1f;
    in->posX = -1.0f;
    in->posY = -1.0f;
    in->scaleX = 0.00075f;
    in->scaleY = 0.00075f;
    in->sprite = global.sprite[SP_PLAYER];

    //Create bounding box for player


    //Add BSP
    BSP* bsp = (BSP*)Pool_NewItem(&global.pool[PL_BSP]);
    Thread* bspThread = Thread_Init(TH_BSP, nullptr);
    BSP_Init(bsp, 8, 2);
}

void Level_ResetCoins()
{
    Instance* player = reinterpret_cast<Instance*>(global.pool[PL_INSTANCE].linkedList_TakenItems);
    BSP* bsp = reinterpret_cast<BSP*>(global.pool[PL_BSP].linkedList_TakenItems);

    //Clear old BSP
    BSP_Clear_Node(bsp);

    //Add new items to BSP
    BSP_Fill(bsp, player);

    // spawn 5 coins
    for (int i = 0; i < 5; i++)
    {
        // Notice now we dont need an array,
        // because threads are linked, so we
        // can lose the pointers and still 
        // access the data later when we need it
        Instance* in = (Instance*)Pool_NewItem(&global.pool[PL_INSTANCE]);

        // coin doesn't need an update function,
        // but we will use it's onCollide pointer
        // in future tutorials
        Thread* th = Thread_Init(TH_COIN, nullptr);
        th->funcOnCollide = Coin_OnCollide;

        // link instance and thread
        th->inst = in;
        in->thread = th;

        // draw coins on bottom (larger depth)
        in->depth = 0.2f;
        in->posX = ((float)rand() / RAND_MAX) * 1.5f - 0.75f; // random number -0.75 to 0.75
        in->posY = ((float)rand() / RAND_MAX) * 1.5f - 0.75f; // random number -0.75 to 0.75
        in->scaleX = 0.002f;
        in->scaleY = 0.002f;
        in->sprite = global.sprite[SP_COIN];

        BSP_Fill(bsp, in);
    }

    BSP_Generate(bsp);
}

void Level_Update()
{
    // if there are no coins remaining
    if (global.threadBucket[TH_COIN] == 0)
    {
        // reset the game
        Level_ResetCoins();
    }
}