#pragma once
#include "pch.h"

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

	bool split;

	std::vector<Instance*>* instances;

	BSP* parent;
	BSPChildren children;

	BoundingBox boundingBox;
	float meanContentsValue; //determines where the BSP should split
	glm::vec2 topLeft;
	glm::vec2 bottomRight;
};

//Add instances that will be split into
//BSP nodes when 'Generate_BSP' is called
void BSP_Fill(BSP* bsp, std::vector<Instance*> instances);

void BSP_Fill(BSP* bsp, Instance* instance);

//Creates a BSP tree from this node downward
void BSP_Generate(BSP* bsp);

//Will appropriately size each BSP node if they
//have more instances than needed to split 
//(don't call directly unless you know what you are doing)
void BSP_Smart_Distribute_Down(BSP* bsp);

//Resize a root node with no children, nothing else
void BSP_Basic_Resize(BSP* bsp, Instance* instance);

//Collects all of the instances in the BSP's children and
//puts them in childInstances
void BSP_Collect_Children(BSP* bsp, std::vector<Instance*>* childInstances);

//Self explainatory
void BSP_Generate_Children(BSP* bsp);

//Deletes all of the data in this node and all nodes below
void BSP_Clear_Node(BSP* bsp, bool root = true);

//Sets up a BSP node to be a head node
void BSP_Init(BSP* bsp, int maxSubdivisions, int minToSubdivide);

//Call this to put an instance in an
//already created BSP tree
void BSP_Place_Into(BSP* bsp, Instance* inst);

//Places into a 2d bsp only
void BSP_Place_Into_2D(BSP* bsp, Instance* instance);

BSP* BSP_Create_Root(int maxSubdivisions, int minToSubdivide);

