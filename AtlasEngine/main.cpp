#include "pch.h"

struct Global global;

int main()
{
    printf("Globals: %d bytes\n", sizeof(global));

    // this should be the one and only "malloc"
    void* mempackRamData = malloc(ramSize);

    // initialize mempack for RAM
    MEMPACK_Init(&global.ramPack, mempackRamData, ramSize, "ram");

    // create a window, create context,
    // create GPU Mempacks, etc
    InitOpenGL();

    // seed random number generator
    srand(time(NULL));

    // initialize level
    Level_Init();

    // timer variables
    clock_t start;
    clock_t end;

    // initialize time
    start = clock();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(global.window))
    {
        // measure elapsed time per frame
        end = clock();
        global.elapsedTime = (end - start) / 1000.0f;
        start = clock();

        // Update controller "hold" variable
        glfwPollEvents();

        // Determine Tap and Release
        Input_ProcessState();

        // update level
        Level_Update();

        // do all thread work
        Engine_HandleThreadUpdates();
        Engine_HandleThreadCollisions();
        Engine_HandleThreadDeaths();
        
        // do all rendering work
        Engine_HandleRendering();
    }

    // erase all allocated data
    MEMPACK_Clean(&global.ramPack);

    // erase resources
    delete mempackRamData;
    mempackRamData = nullptr;

    CleanOpenGL();

    // end program
    return 0;
}