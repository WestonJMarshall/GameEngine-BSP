#include "pch.h"

void Engine_HandleThreadUpdates()
{
    // Loop through all threads in use
    Thread* th = (Thread*)global.pool[PL_THREAD].linkedList_TakenItems;
    while (th != nullptr)
    {
        if (th->funcOnTick != nullptr)
        {
            // execute thread's function,
            // passing itself as a parameter
            th->funcOnTick(th);
        }

        // go to next thread
        th = th->next;
    }
}

void Engine_HandleThreadCollisions()
{
    // Check for all collisions between all players and all coins
    Thread* playerTh = global.threadBucket[TH_PLAYER];
    while (playerTh != nullptr)
    {
        Thread* coinTh = global.threadBucket[TH_COIN];
        while (coinTh != nullptr)
        {
            // check for collisions
            CheckCollision_Threads(playerTh, coinTh);

            coinTh = coinTh->siblingThread;
        }

        playerTh = playerTh->siblingThread;
    }
}

void Engine_HandleThreadDeaths()
{
    // Loop through all thread buckets, look for dead threads
    for (int i = 0; i < TH_NUM_TYPES; i++)
    {
        Thread* th = global.threadBucket[i];

        // continue until the first thread 
        // in the bucket is alive
        while (th != nullptr && th->flags & TH_FLAG_DEAD)
        {
            // kill thread
            Pool_EraseItem(&global.pool[PL_THREAD], (Item*)th);

            // kill instance, if it exists
            if (th->inst != nullptr)
            {
                Pool_EraseItem(&global.pool[PL_INSTANCE], (Item*)th->inst);
            }

            // if all threads die, threadBucket
            // will be nullptr, and that's ok
            global.threadBucket[i] = th->siblingThread;

            // check the next thread
            th = th->siblingThread;
        }

        while (th != nullptr)
        {
            // if next thread exists
            if (th->siblingThread != nullptr)
            {
                // if next thread is dead
                if (th->siblingThread->flags & TH_FLAG_DEAD)
                {
                    // kill thread and instance
                    Pool_EraseItem(&global.pool[PL_INSTANCE], (Item*)th->siblingThread->inst);
                    Pool_EraseItem(&global.pool[PL_THREAD], (Item*)th->siblingThread);

                    // link to the next sibling, removing dead thread from linked list,
                    // there is nothing wrong with this becoming nullptr
                    th->siblingThread = th->siblingThread->siblingThread;
                }
            }

            // check next thread in bucket
            th = th->siblingThread;
        }
    }
}

void Engine_HandleRendering()
{
    // render
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Loop through all threads in use
    Instance* in = (Instance*)global.pool[PL_INSTANCE].linkedList_TakenItems;
    while (in != nullptr)
    {
        Instance_Draw(in);

        // go to next thread
        in = in->next;
    }

    // update swapchain
    glfwSwapBuffers(global.window);
}