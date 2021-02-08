#include "pch.h"
#include "BSP.h"

void Fill_BSP(BSP* bsp, Instance* inst, int instanceCount = 1)
{
	if (!bsp->initialized) return;

	//TODO Add the instances to the bsp, do nothing else
}

BSP* Generate_BSP(BSP* bsp)
{
	if (!bsp->initialized) return;

	//TODO Take all other instances below this node and add it to this one, then delete all child nodes

	//TODO split this node into children as neccessary
}

void Init_BSP(BSP* bsp, int maxSubdivisions, int minToSubdivide)
{
	bsp->maxSubdivisions = maxSubdivisions;
	bsp->minToSubdivide = minToSubdivide;
	bsp->subdivisionLevel = 0;
	bsp->split = false;
	bsp->numInstances = 0;
	bsp->meanContentsValue = 0.0f;

	bsp->boundingBox.back = 0.0f;
	bsp->boundingBox.front = 0.0f;
	bsp->boundingBox.left = 0.0f;
	bsp->boundingBox.right = 0.0f;
	bsp->boundingBox.top = 0.0f;
	bsp->boundingBox.bottom = 0.0f;

	bsp->initialized = true;

	//TODO Remove children and parents if there are any
}

void Place_Into_BSP(BSP* bsp, Instance* inst)
{
	if (!bsp->initialized) return;

	//TODO If this is the head node and the instance is outside of its bounds, we have to regenerate everything



	//TODO if there are child nodes we will put this instance in one (or both???) of them (call pib and quit out)



	//TODO else If the current node does not have children, add the instance, increment the meanContentsValue, and check if a split is neccessary



}

void Clear_BSP_Node(BSP* bsp)
{
	if (!bsp->initialized) return;

	//Remove all instances from this node downwards, then delete all of this node's children

}

