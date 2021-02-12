#pragma once
#include "pch.h"

//In the future this may not be axis aligned
enum class BSPSplitDirection
{
	leftRight,
	frontBack,
	topBottom
};

struct BSP
{
	struct BSPChildren
	{
		BSP* child1;
		BSP* child2;
	};

	bool initialized = false;

	BSPSplitDirection type;

	unsigned int subdivisionLevel;
	unsigned int maxSubdivisions;
	unsigned int minToSubdivide;

	std::vector<Instance*>* instances;

	BSP* parent;
	BSPChildren children;

	BoundingBox boundingBox;
	float meanContentsValue; //determines where the BSP should split
};

//INIT | FILL | GENERATE | CLEAR are the only functions that are likely to be used outside of BSP.cpp, Place_Into can also be used if you know what you are doing

//Add instances to a node but do not subdivide.
//BSP will split when when 'BSP_Generate' is called
void BSP_Fill(BSP* bsp, Instance* instance);

//Creates a BSP tree from this node downward
void BSP_Generate(BSP* bsp);

//Will appropriately size each BSP node if they
//have more instances than needed to split 
void BSP_Distribute_Down(BSP* bsp);

//Resize a root node with no children, nothing else
void BSP_Basic_Resize(BSP* bsp, Instance* instance);

//Collects all of the instances in the BSP's children and
//puts them in childInstances
void BSP_Collect_Children(BSP* bsp, std::vector<Instance*>* childInstances);

//Self explainatory
void BSP_Generate_Children(BSP* bsp);

//Deletes all of the data in this node and all nodes below
void BSP_Clear_Node(BSP* bsp, bool root = true);

//Resets the bounds on the BSP
void BSP_Clear_Resizing(BSP* bsp);

//Sets up a BSP node to be a head node
void BSP_Init(BSP* bsp, int maxSubdivisions, int minToSubdivide);

//Call this to put an instance in an
//already created BSP tree
void BSP_Place_Into(BSP* bsp, Instance* inst);


//2D exclusive functions V


//Creates a BSP tree from this node downward
void BSP_Generate_2D(BSP* bsp);

//Will appropriately size each BSP node if they
//have more instances than needed to split 
void BSP_Distribute_Down_2D(BSP* bsp);

//Self explainatory
void BSP_Generate_Children_2D(BSP* bsp);

//Places into a 2d bsp only
void BSP_Place_Into_2D(BSP* bsp, Instance* instance);

