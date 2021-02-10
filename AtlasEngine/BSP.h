#pragma once
#include "pch.h"

bool Bounding_Box_Collision(BoundingBox& b1, BoundingBox& b2)
{
	if (b1.left > b2.right
		|| b1.right < b2.left
		|| b1.front < b2.back
		|| b1.back > b2.front
		|| b1.top < b2.bottom
		|| b1.bottom > b2.top)
	{
		return false;
	}
	return true;
}

bool Bounding_Box_Collision2D(BoundingBox& b1, BoundingBox& b2)
{
	if (b1.left > b2.right
		|| b1.right < b2.left
		|| b1.top < b2.bottom
		|| b1.bottom > b2.top)
	{
		return false;
	}
	return true;
}

enum BSPSplitDirection
{
	leftRight,
	frontBack,
	topBottom
};

//for 2d bsp checking
//enum BSPSplitDirection2D
//{
	//leftRight,
	//topBottom
//};

struct BSP
{
	struct BSPChildren
	{
		BSP* child1;
		BSP* child2;
	};

	bool initialized = false;

	BSPSplitDirection type;

	int subdivisionLevel;

	int maxSubdivisions;
	int minToSubdivide;

	bool split;

	std::vector<Instance*> instances;

	BSP* parent;
	BSPChildren children;

	BoundingBox boundingBox;
	float meanContentsValue; //determines where the BSP should split
	glm::vec2 topLeft;
	glm::vec2 bottomRight;
};



//Add instances that will be split into
//BSP nodes when 'Generate_BSP' is called
void Fill_BSP(BSP* bsp, std::vector<Instance*> instances);

void Fill_BSP(BSP* bsp, Instance* instance);

//Creates a BSP tree from this node downward
void Generate_BSP(BSP* bsp);

//Will appropriately size each BSP node if they
//have more instances than needed to split 
//(don't call directly unless you know what you are doing)
void Smart_Distribute_Down_BSP(BSP* bsp);

//Collects all of the instances in the BSP's children and
//puts them in childInstances
void Collect_BSP_Children(BSP* bsp, std::vector<Instance*>* childInstances);

//Self explainatory
void Generate_BSP_Children(BSP* bsp);

//Deletes all of the data in this node and all nodes below
void Clear_BSP_Node(BSP* bsp);

//Sets up a BSP node to be a head node
void Init_BSP(BSP* bsp, int maxSubdivisions, int minToSubdivide);

//Call this to put an instance in an
//already created BSP tree
void Place_Into_BSP(BSP* bsp, Instance* inst);

//Places into a 2d bsp only
void Place_Into_BSP2D(BSP* bsp, Instance* instance);

BSP* Create_Root_BSP(int maxSubdivisions, int minToSubdivide);

