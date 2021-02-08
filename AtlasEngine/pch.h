#pragma once

// This should be a pch at some point

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <time.h>

// fowrad declaration
struct Mempack;
struct Sprite;
struct Instance;
struct Thread;
struct Player;
struct Item;
struct Pool;

#include "mempack.h"
#include "graphics.h"
#include "sprite.h"
#include "instance.h"
#include "input.h"
#include "thread.h"
#include "player.h"
#include "pool.h"
#include "collision.h"
#include "coin.h"
#include "level.h"
#include "engine.h"

struct Global
{
	GLFWwindow* window;

	// opengl types
	GLuint VBO, VAO, PBO, program;

	// stack allocators
	Mempack ramPack;
	Mempack vboPack;
	Mempack pboPack;

	// input system
	Keyboard keys;

	// pointers because these are linked lists
	Thread* threadBucket[TH_NUM_TYPES];
	
	// not pointer, but contain pointers inside
	Pool pool[PL_NUM_TYPES];

	// pointer to every kind of sprite
	Sprite* sprite[SP_NUM_TYPES];

	float elapsedTime;
};

extern Global global;

// Right now we allocate almost nothing, but
// the dial will be cranked up in future tutorials,
// when we add more stuff, and get to 3D graphics.
// You can allocate several gigabytes if you want
#define vboSize 0x1000   // 0001 kilobytes <-- just a quad
#define pboSize 0x100000 // 1024 kilobytes
#define ramSize 0x100000 // 1024 kilobytes

// For industry-level engines, please compile
// in x64 instead of x86. All the tutorials are
// x86 because some people dont own x64 computers

// When compiling in x86 mode, malloc fails with
// any amount 2gb or more, 1gb - 1.9gb return a pointer,
// but the engine fails to run if you malloc 1.7gb+