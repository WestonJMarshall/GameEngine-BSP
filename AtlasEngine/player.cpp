#include "pch.h"

void Player_OnTick(Thread* th)
{
    Instance* in = th->inst;

    if (Input_KeyHold('A'))
        in->posX -= global.elapsedTime;

    if (Input_KeyHold('D'))
        in->posX += global.elapsedTime;

    if (Input_KeyHold('W'))
        in->posY += global.elapsedTime;

    if (Input_KeyHold('S'))
        in->posY -= global.elapsedTime;

    // Just for a test
    if (Input_KeyTap('T')) printf("Tap T\n");
    if (Input_KeyHold('Y')) printf("Hold Y\n");
    if (Input_KeyRelease('U')) printf("Release U\n");

    // count all coins
    if (Input_KeyTap('C'))
    {
        int count = 0;

        Thread* th = global.threadBucket[TH_COIN];

        while (th != nullptr)
        {
            count++;
            th = th->siblingThread;
        }

        printf("there are %d coins left\n", count);
    }
}