#include "pch.h"
#include "BSP.h"

void Fill_BSP(BSP* bsp, Instance* inst, int instanceCount)
{
	if (!bsp->initialized) return;

	//TODO Add the instances to the bsp, do nothing else
}

BSP* Generate_BSP(BSP* bsp)
{
	if (!bsp->initialized) return nullptr;

	return nullptr;

	//TODO Take all other instances below this node and add it to this one, then delete all child nodes

	//TODO split this node into children as neccessary
}

void Init_BSP(BSP* bsp, int maxSubdivisions, int minToSubdivide)
{
	bsp->type = leftRight;

	bsp->maxSubdivisions = maxSubdivisions;
	bsp->minToSubdivide = minToSubdivide;
	bsp->subdivisionLevel = 0;
	bsp->split = false;
	bsp->meanContentsValue = 0.0f;

	//TEMP
	bsp->instances.clear();

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
	if (bsp->children.child1 != nullptr)
	{
		if (Bounding_Box_Collision(bsp->boundingBox, bsp->children.child1->boundingBox))
		{
			Place_Into_BSP(bsp->children.child1, inst);
		}
		else
		{
			Place_Into_BSP(bsp->children.child2, inst);
		}
		return;
	}

	//TODO else If the current node does not have children, add the instance, increment the meanContentsValue, and check if a split is neccessary
	else
	{
		bsp->instances.push_back(inst);

		//The type of BSP determines the next action
		if (bsp->type == leftRight)
		{
			bsp->meanContentsValue += ((inst->boundingBox.right + inst->boundingBox.left) / 2.0f);

			//Split BSP if neccessary
			if (bsp->instances.size() > bsp->minToSubdivide)
			{
				float splitLoc = bsp->meanContentsValue / bsp->instances.size();

				BSP child1{};
				Init_BSP(&child1, bsp->maxSubdivisions, bsp->minToSubdivide);

				child1.type = frontBack;
				child1.parent = bsp;

				child1.boundingBox.left = bsp->boundingBox.left;
				child1.boundingBox.right = splitLoc;
				child1.boundingBox.front = bsp->boundingBox.front;
				child1.boundingBox.back = bsp->boundingBox.back;
				child1.boundingBox.top = bsp->boundingBox.top;
				child1.boundingBox.bottom = bsp->boundingBox.bottom;

				bsp->children.child1 = &child1;

				//2
				BSP child2{};
				Init_BSP(&child2, bsp->maxSubdivisions, bsp->minToSubdivide);

				child2.type = frontBack;
				child2.parent = bsp;

				child2.boundingBox.left = splitLoc;
				child2.boundingBox.right = bsp->boundingBox.right;
				child2.boundingBox.front = bsp->boundingBox.front;
				child2.boundingBox.back = bsp->boundingBox.back;
				child2.boundingBox.top = bsp->boundingBox.top;
				child2.boundingBox.bottom = bsp->boundingBox.bottom;

				bsp->children.child2 = &child2;

				//Place instances into newly created child nodes
				for (auto& i : bsp->instances)
				{
					Place_Into_BSP(bsp, i);
				}
			}
		}
		else if (bsp->type == frontBack)
		{
			bsp->meanContentsValue += ((inst->boundingBox.front + inst->boundingBox.back) / 2.0f);

			//Split BSP if neccessary
			if (bsp->instances.size() > bsp->minToSubdivide)
			{
				float splitLoc = bsp->meanContentsValue / bsp->instances.size();

				BSP child1{};
				Init_BSP(&child1, bsp->maxSubdivisions, bsp->minToSubdivide);

				child1.type = topBottom;
				child1.parent = bsp;

				child1.boundingBox.left = bsp->boundingBox.left;
				child1.boundingBox.right = bsp->boundingBox.right;
				child1.boundingBox.front = splitLoc;
				child1.boundingBox.back = bsp->boundingBox.back;
				child1.boundingBox.top = bsp->boundingBox.top;
				child1.boundingBox.bottom = bsp->boundingBox.bottom;

				bsp->children.child1 = &child1;

				//2
				BSP child2{};
				Init_BSP(&child2, bsp->maxSubdivisions, bsp->minToSubdivide);

				child2.type = topBottom;
				child2.parent = bsp;

				child2.boundingBox.left = bsp->boundingBox.left;
				child2.boundingBox.right = bsp->boundingBox.right;
				child2.boundingBox.front = bsp->boundingBox.front;
				child2.boundingBox.back = splitLoc;
				child2.boundingBox.top = bsp->boundingBox.top;
				child2.boundingBox.bottom = bsp->boundingBox.bottom;

				bsp->children.child2 = &child2;

				//Place instances into newly created child nodes
				for (auto& i : bsp->instances)
				{
					Place_Into_BSP(bsp, i);
				}
			}
		}
		else //topBottom
		{
			bsp->meanContentsValue += ((inst->boundingBox.top + inst->boundingBox.bottom) / 2.0f);

			//Split BSP if neccessary
			if (bsp->instances.size() > bsp->minToSubdivide)
			{
				float splitLoc = bsp->meanContentsValue / bsp->instances.size();

				BSP child1{};
				Init_BSP(&child1, bsp->maxSubdivisions, bsp->minToSubdivide);

				child1.type = leftRight;
				child1.parent = bsp;

				child1.boundingBox.left = bsp->boundingBox.left;
				child1.boundingBox.right = bsp->boundingBox.right;
				child1.boundingBox.front = bsp->boundingBox.front;
				child1.boundingBox.back = bsp->boundingBox.back;
				child1.boundingBox.top = splitLoc;
				child1.boundingBox.bottom = bsp->boundingBox.bottom;

				bsp->children.child1 = &child1;

				//2
				BSP child2{};
				Init_BSP(&child2, bsp->maxSubdivisions, bsp->minToSubdivide);

				child2.type = leftRight;
				child2.parent = bsp;

				child2.boundingBox.left = bsp->boundingBox.left;
				child2.boundingBox.right = bsp->boundingBox.right;
				child2.boundingBox.front = bsp->boundingBox.front;
				child2.boundingBox.back = bsp->boundingBox.back;
				child2.boundingBox.top = bsp->boundingBox.top;
				child2.boundingBox.bottom = splitLoc;

				bsp->children.child2 = &child2;

				//Place instances into newly created child nodes
				for (auto& i : bsp->instances)
				{
					Place_Into_BSP(bsp, i);
				}
			}
		}
	}
}

void Clear_BSP_Node(BSP* bsp)
{
	if (!bsp->initialized) return;

	//Remove all instances from this node downwards, then delete all of this node's children

}

