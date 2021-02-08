#pragma once

struct BoundingBox
{
	float left;
	float right;
	float top;
	float bottom;
	float front;
	float back;
};

enum BSPSplitDirection
{
	leftRight,
	frontBack,
	topBottom
};

struct BSP
{
	bool initialized = false;

	int subdivisionLevel;

	int maxSubdivisions;
	int minToSubdivide;

	bool split;

	int numInstances;
	Instance* instances;

	BSP* root;
	BSP* parent;
	BSP* children;

	BoundingBox boundingBox;
	float meanContentsValue; //determines where the BSP should split (-1 to 1)
};

//Add instances that will be split into
//BSP nodes when 'Generate_BSP' is called
void Fill_BSP(BSP* bsp, Instance* inst, int instanceCount = 1);

//Creates a BSP tree from this node downward
BSP* Generate_BSP(BSP* bsp);

//Deletes all of the data in this node and all nodes below
void Clear_BSP_Node(BSP* bsp);

//Sets up a BSP node to be a head node
void Init_BSP(BSP* bsp, int maxSubdivisions, int minToSubdivide);

//Call this to put an instance in an
//already created BSP tree
void Place_Into_BSP(BSP* bsp, Instance* inst);

