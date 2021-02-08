Documentation Author: Niko Procopi 2020

This tutorial was designed for Visual Studio 2019
If the solution does not compile, retarget the solution
to a different version of the Windows SDK. If you do not
have any version of the Windows SDK, it can be installed
from the Visual Studio Installer Tool

AtlasEngine - Collisions
Prerequisites
	Intro C tutorials, Basic OpenGL tutorials, and 
	physics tutorials are required before starting
	the engine series

main.cpp is finally clean and organized,
the whole "game" consists of Level, Player, and Coin cpp files,
all pieces of gameplay can be changed without looking at engine code

engine.cpp uses thread buckets to update all threads, 
check for collision between items, and destroy threads
when necessary. All Instances in the scene by using Instance Pool,
and this can all be abstracted from gameplay code.

level.cpp handles initializing, updating, and resetting the level.
Level_Init first loads sprites and initializes pools.

Then creates and Instance and Thread for Player, by setting 
Player's update function to Player_OnTick, to update each frame.

Then Instances and Threads are made for Coin, by setting no 
update function, and setting collision function to Coin_OnCollide

Finally, Level_Update restarts the scene when all 
the coins are collected. 

Coming Up:
	Build a pools for allocating data of any kind, rather than having,
one pool restricted to each type of data. This will allow threads to
have data "attached" to them, such as storing variables in player for 
health, lives, etc. This is how Crash Team Racing handles data attached
to threads, such as data related to players, data related to weapons 
placed on tracks, etc