#include <iostream>
#include "pch.h"

#pragma region Utility
void BSP_Init(BSP* bsp, int maxSubdivisions, int minToSubdivide)
{
	bsp->type = BSPSplitDirection::leftRight;

	bsp->maxSubdivisions = maxSubdivisions;
	bsp->minToSubdivide = minToSubdivide;
	bsp->subdivisionLevel = 0;
	bsp->meanContentsValue = 0.0f;

	bsp->children.child1 = nullptr;
	bsp->children.child2 = nullptr;

	bsp->instances = new std::vector<Instance*>();

	bsp->initialized = true;

	//Remove children and parents if there are any
	BSP_Clear_Node(bsp);
	BSP_Clear_Resizing(bsp);
}

void BSP_Fill(BSP* bsp, Instance* instance)
{
	if (!bsp->initialized) return;

	//Add the instances to the bsp, resize, then do nothing else
	bsp->instances->push_back(instance);
	BSP_Basic_Resize(bsp, instance);
}

void BSP_Basic_Resize(BSP* bsp, Instance* instance)
{
	//If this is the head node and the instance is outside of its bounds, make it bigger
	if (bsp->subdivisionLevel == 0)
	{
		if (instance->boundingBox.left < bsp->boundingBox.left) { bsp->boundingBox.left = instance->boundingBox.left - std::numeric_limits<float>::epsilon(); }
		if (instance->boundingBox.right > bsp->boundingBox.right) { bsp->boundingBox.right = instance->boundingBox.right + std::numeric_limits<float>::epsilon(); }
		if (instance->boundingBox.bottom < bsp->boundingBox.bottom) { bsp->boundingBox.bottom = instance->boundingBox.bottom - std::numeric_limits<float>::epsilon(); }
		if (instance->boundingBox.top > bsp->boundingBox.top) { bsp->boundingBox.top = instance->boundingBox.top + std::numeric_limits<float>::epsilon(); }
		if (instance->boundingBox.back < bsp->boundingBox.back) { bsp->boundingBox.back = instance->boundingBox.back - std::numeric_limits<float>::epsilon(); }
		if (instance->boundingBox.front > bsp->boundingBox.front) { bsp->boundingBox.front = instance->boundingBox.front + std::numeric_limits<float>::epsilon(); }
	}
}

void BSP_Collect_Children(BSP* bsp, std::vector<Instance*>* childInstances)
{
	if (bsp->children.child1 != nullptr)
	{
		//while child1 exits, loop through the child and get all the instances
		//and add them to the childInstances vector
		for (auto& i : *bsp->children.child1->instances)
		{
			childInstances->push_back(i);
		}
		//recursively call the function to traves the tree of child bsps,
		//and collect their children, function continues past recursive call
		BSP_Collect_Children(bsp->children.child1, childInstances);

		//do the same for child2 as above
		for (auto& i : *bsp->children.child2->instances)
		{
			childInstances->push_back(i);
		}
		BSP_Collect_Children(bsp->children.child2, childInstances);
	}
}

void BSP_Clear_Node(BSP* bsp, bool root)
{
	if (!bsp->initialized) return;

	//Find children
	if (bsp->children.child1 != nullptr)
	{
		BSP_Clear_Node(bsp->children.child1, false);
		BSP_Clear_Node(bsp->children.child2, false);

		//Delete children
		delete bsp->children.child1;
		delete bsp->children.child2;

		bsp->children.child1 = nullptr;
		bsp->children.child2 = nullptr;
	}

	//Remove instances in root node
	if (root)
	{
		bsp->instances->clear();
	}
}

void BSP_Clear_Resizing(BSP* bsp)
{
	//Remove Sizing
	bsp->boundingBox.left = std::numeric_limits<float>::max();
	bsp->boundingBox.right = std::numeric_limits<float>::min();
	bsp->boundingBox.back = std::numeric_limits<float>::max();
	bsp->boundingBox.front = std::numeric_limits<float>::min();
	bsp->boundingBox.bottom = std::numeric_limits<float>::max();
	bsp->boundingBox.top = std::numeric_limits<float>::min();
}
#pragma endregion

#pragma region 3D BSP Functions
void BSP_Generate(BSP* bsp)
{
	if (!bsp->initialized) return;

	//Take all other instances below this node and add it to this one, then delete all child nodes
	std::vector<Instance*>* childInstances = new std::vector<Instance*>();
	BSP_Collect_Children(bsp, childInstances);

	for (auto& i : *bsp->instances)
	{
		childInstances->push_back(i);
	}

	//Remove all instance data in this node and below. Now all instances will be stored in 'childInstances' alone
	BSP_Clear_Node(bsp);

	//Put all the collected instances in the cleared node, then split this node into children as neccessary
	for (auto& i : *childInstances)
	{
		bsp->instances->push_back(i);
	}
	BSP_Distribute_Down(bsp);

	//cleanup
	delete childInstances;
}

void BSP_Distribute_Down(BSP* bsp)
{
	printf("Num Children: %ld ", bsp->instances->size());
	printf("Subdivision Level: %ld \n", (float*)bsp->subdivisionLevel);

	//Average the position values of this node's instances,
	//so this node will split in the middle of all its instances along the appropriate axis
	if (bsp->type == BSPSplitDirection::leftRight)
	{
		for (auto& i : *bsp->instances) 
		{
			bsp->meanContentsValue += ((i->boundingBox.right + i->boundingBox.left) / 2.0f);
		}
	}
	else if (bsp->type == BSPSplitDirection::frontBack)
	{
		for (auto& i : *bsp->instances)
		{
			bsp->meanContentsValue += ((i->boundingBox.front + i->boundingBox.back) / 2.0f);
		}
	}
	else
	{
		for (auto& i : *bsp->instances)
		{
			bsp->meanContentsValue += ((i->boundingBox.top + i->boundingBox.bottom) / 2.0f);
		}
	}

	//Subdivide
	if (bsp->instances->size() > bsp->minToSubdivide && bsp->subdivisionLevel < bsp->maxSubdivisions) 
	{
		BSP_Generate_Children(bsp);

		for (auto& i : *bsp->instances)
		{
			//if the bsp's child1 collides with the bounding box if the instance
			//add the instance to the child bsp, and removed from parent bsp
			if (Bounding_Box_Collision(i->boundingBox, bsp->children.child1->boundingBox))
			{
				printf("Placing instance in child 1\n");
				BSP_Fill(bsp->children.child1, i);
			}
			//if the instance does not collide with the child1 bsp, assume the
			//instance collides with the child2 bsp
			else
			{
				printf("Placing instance in child 2\n");
				BSP_Fill(bsp->children.child2, i);
			}
		}

		bsp->instances->clear();

		BSP_Distribute_Down(bsp->children.child1);
		BSP_Distribute_Down(bsp->children.child2);
	}
}

void BSP_Generate_Children(BSP* bsp) //Add new children to a node
{
	float splitLoc = bsp->meanContentsValue / bsp->instances->size();

	BSP* child1 = new BSP();
	BSP_Init(child1, bsp->maxSubdivisions, bsp->minToSubdivide);
	child1->parent = bsp;
	child1->subdivisionLevel = bsp->subdivisionLevel + 1;

	child1->boundingBox.left = bsp->boundingBox.left;
	child1->boundingBox.right = bsp->boundingBox.right;
	child1->boundingBox.front = bsp->boundingBox.front;
	child1->boundingBox.back = bsp->boundingBox.back;
	child1->boundingBox.top = bsp->boundingBox.top;
	child1->boundingBox.bottom = bsp->boundingBox.bottom;

	BSP* child2 = new BSP();
	BSP_Init(child2, bsp->maxSubdivisions, bsp->minToSubdivide);
	child2->parent = bsp;
	child2->subdivisionLevel = bsp->subdivisionLevel + 1;

	child2->boundingBox.left = bsp->boundingBox.left;
	child2->boundingBox.right = bsp->boundingBox.right;
	child2->boundingBox.front = bsp->boundingBox.front;
	child2->boundingBox.back = bsp->boundingBox.back;
	child2->boundingBox.top = bsp->boundingBox.top;
	child2->boundingBox.bottom = bsp->boundingBox.bottom;

	//depending on the split direction, set the children bsp's values 
	//to the split location and iterate to the next enum value of split
	//direction, the order is LeftRight->FrontBack->TopBottom
	if (bsp->type == BSPSplitDirection::leftRight)
	{
		child1->type = BSPSplitDirection::frontBack;
		child1->boundingBox.right = splitLoc;

		child2->type = BSPSplitDirection::frontBack;
		child2->boundingBox.left = splitLoc;
	}
	else if (bsp->type == BSPSplitDirection::frontBack)
	{
		child1->type = BSPSplitDirection::topBottom;
		child1->boundingBox.front = splitLoc;

		child2->type = BSPSplitDirection::topBottom;
		child2->boundingBox.back = splitLoc;
	}
	else
	{
		child1->type = BSPSplitDirection::leftRight;
		child1->boundingBox.top = splitLoc;

		child2->type = BSPSplitDirection::leftRight;
		child2->boundingBox.bottom = splitLoc;
	}

	bsp->children.child1 = child1;
	bsp->children.child2 = child2;
}

void BSP_Place_Into(BSP* bsp, Instance* inst)
{
	if (!bsp->initialized) return;

	//If this is the head node and the instance is outside of its bounds, we have to regenerate everything
	if (bsp->subdivisionLevel == 0)
	{
		bool regenerate = false;
		if (inst->boundingBox.left < bsp->boundingBox.left) { regenerate = true; bsp->boundingBox.left = inst->boundingBox.left - std::numeric_limits<float>::epsilon(); }
		if (inst->boundingBox.right > bsp->boundingBox.right) { regenerate = true; bsp->boundingBox.right = inst->boundingBox.right + std::numeric_limits<float>::epsilon(); }
		if (inst->boundingBox.bottom < bsp->boundingBox.bottom) { regenerate = true; bsp->boundingBox.bottom = inst->boundingBox.bottom - std::numeric_limits<float>::epsilon(); }
		if (inst->boundingBox.top > bsp->boundingBox.top) { regenerate = true; bsp->boundingBox.top = inst->boundingBox.top + std::numeric_limits<float>::epsilon(); }
		if (inst->boundingBox.back < bsp->boundingBox.back) { regenerate = true; bsp->boundingBox.back = inst->boundingBox.back - std::numeric_limits<float>::epsilon(); }
		if (inst->boundingBox.front > bsp->boundingBox.front) { regenerate = true; bsp->boundingBox.front = inst->boundingBox.front + std::numeric_limits<float>::epsilon(); }

		if (regenerate)
		{
			bsp->instances->push_back(inst);
			BSP_Generate(bsp);
			return;
		}
	}

	//if there are child nodes we will put this instance in one (or both???) of them (call pib and quit out)
	if (bsp->children.child1 != nullptr)
	{
		//if an instance collides with the child bsp, place the instance inside of the child BSP
		if (Bounding_Box_Collision(inst->boundingBox, bsp->children.child1->boundingBox))
		{
			BSP_Place_Into(bsp->children.child1, inst);
		}
		else
		{
			BSP_Place_Into(bsp->children.child2, inst);
		}
		return;
	}
	//else If the current node does not have children, add the instance, increment the meanContentsValue, and check if a split is neccessary
	else
	{
		bsp->instances->push_back(inst);

		//The type of BSP split direction determines the next action
		//and will split the bounding box based on the direction
		if (bsp->type == BSPSplitDirection::leftRight)
		{
			bsp->meanContentsValue += ((inst->boundingBox.right + inst->boundingBox.left) / 2.0f);
		}
		else if (bsp->type == BSPSplitDirection::frontBack)
		{
			bsp->meanContentsValue += ((inst->boundingBox.front + inst->boundingBox.back) / 2.0f);
		}
		else //topBottom
		{
			bsp->meanContentsValue += ((inst->boundingBox.top + inst->boundingBox.bottom) / 2.0f);
		}

		//Split BSP if neccessary
		if (bsp->instances->size() > bsp->minToSubdivide)
		{
			BSP_Generate_Children(bsp);

			//Place instances into newly created child nodes
			for (auto& i : *bsp->instances)
			{
				BSP_Place_Into(bsp, i);
			}

			//Clear this nodes instances
			bsp->instances->clear();
		}
	}
}
#pragma endregion

#pragma region 2D BSP Functions
void BSP_Generate_2D(BSP* bsp)
{
	if (!bsp->initialized) return;

	//Take all other instances below this node and add it to this one, then delete all child nodes
	std::vector<Instance*>* childInstances = new std::vector<Instance*>();
	BSP_Collect_Children(bsp, childInstances);

	for (auto& i : *bsp->instances)
	{
		childInstances->push_back(i);
	}

	//Remove all instance data in this node and below. Now all instances will be stored in 'childInstances' alone
	BSP_Clear_Node(bsp);

	//Put all the collected instances in the cleared node, then split this node into children as neccessary
	for (auto& i : *childInstances)
	{
		bsp->instances->push_back(i);
	}
	BSP_Distribute_Down_2D(bsp);

	//cleanup
	delete childInstances;
}

void BSP_Distribute_Down_2D(BSP* bsp)
{
	printf("Num Children: %ld ", bsp->instances->size());
	printf("Subdivision Level: %ld \n", (float*)bsp->subdivisionLevel);

	//Average the position values of this node's instances,
	//so this node will split in the middle of all its instances
	if (bsp->type == BSPSplitDirection::leftRight)
	{
		for (auto& i : *bsp->instances)
		{
			bsp->meanContentsValue += ((i->boundingBox.right + i->boundingBox.left) / 2.0f);
		}
	}
	else
	{
		for (auto& i : *bsp->instances)
		{
			bsp->meanContentsValue += ((i->boundingBox.top + i->boundingBox.bottom) / 2.0f);
		}
	}

	//Subdivide
	if (bsp->instances->size() > bsp->minToSubdivide && bsp->subdivisionLevel < bsp->maxSubdivisions)
	{
		BSP_Generate_Children_2D(bsp);

		for (auto& i : *bsp->instances)
		{
			//if the bsp's child1 collides with the bounding box if the instance
			//add the instance to the child bsp, and removed from parent bsp
			if (Bounding_Box_Collision_2D(i->boundingBox, bsp->children.child1->boundingBox))
			{
				printf("Placing instance in child 1\n");
				BSP_Fill(bsp->children.child1, i);
			}
			//if the instance does not collide with the child1 bsp, assume the
			//instance collides with the child2 bsp
			else
			{
				printf("Placing instance in child 2\n");
				BSP_Fill(bsp->children.child2, i);
			}
		}

		bsp->instances->clear();

		BSP_Distribute_Down_2D(bsp->children.child1);
		BSP_Distribute_Down_2D(bsp->children.child2);
	}
}

void BSP_Generate_Children_2D(BSP* bsp) //Add new children to a node
{
	float splitLoc = bsp->meanContentsValue / bsp->instances->size();

	BSP* child1 = new BSP();
	BSP_Init(child1, bsp->maxSubdivisions, bsp->minToSubdivide);
	child1->parent = bsp;
	child1->subdivisionLevel = bsp->subdivisionLevel + 1;

	child1->boundingBox.left = bsp->boundingBox.left;
	child1->boundingBox.right = bsp->boundingBox.right;
	child1->boundingBox.front = bsp->boundingBox.front;
	child1->boundingBox.back = bsp->boundingBox.back;
	child1->boundingBox.top = bsp->boundingBox.top;
	child1->boundingBox.bottom = bsp->boundingBox.bottom;

	BSP* child2 = new BSP();
	BSP_Init(child2, bsp->maxSubdivisions, bsp->minToSubdivide);
	child2->parent = bsp;
	child2->subdivisionLevel = bsp->subdivisionLevel + 1;

	child2->boundingBox.left = bsp->boundingBox.left;
	child2->boundingBox.right = bsp->boundingBox.right;
	child2->boundingBox.front = bsp->boundingBox.front;
	child2->boundingBox.back = bsp->boundingBox.back;
	child2->boundingBox.top = bsp->boundingBox.top;
	child2->boundingBox.bottom = bsp->boundingBox.bottom;

	//depending on the split direction, set the children bsp's values 
	//to the split location and iterate to the next enum value of split
	//direction, the order is LeftRight->TopBottom
	if (bsp->type == BSPSplitDirection::leftRight)
	{
		child1->type = BSPSplitDirection::topBottom;
		child1->boundingBox.right = splitLoc;

		child2->type = BSPSplitDirection::topBottom;
		child2->boundingBox.left = splitLoc;
	}
	else
	{
		child1->type = BSPSplitDirection::leftRight;
		child1->boundingBox.top = splitLoc;

		child2->type = BSPSplitDirection::leftRight;
		child2->boundingBox.bottom = splitLoc;
	}

	bsp->children.child1 = child1;
	bsp->children.child2 = child2;
}

void BSP_Place_Into_2D(BSP* bsp, Instance* inst)
{
	if (!bsp->initialized) return;

	//If this is the head node and the instance is outside of its bounds, we have to regenerate everything
	if (bsp->subdivisionLevel == 0)
	{
		bool regenerate = false;
		if (inst->boundingBox.left < bsp->boundingBox.left) { regenerate = true; bsp->boundingBox.left = inst->boundingBox.left - std::numeric_limits<float>::epsilon(); }
		if (inst->boundingBox.right > bsp->boundingBox.right) { regenerate = true; bsp->boundingBox.right = inst->boundingBox.right + std::numeric_limits<float>::epsilon(); }
		if (inst->boundingBox.bottom < bsp->boundingBox.bottom) { regenerate = true; bsp->boundingBox.bottom = inst->boundingBox.bottom - std::numeric_limits<float>::epsilon(); }
		if (inst->boundingBox.top > bsp->boundingBox.top) { regenerate = true; bsp->boundingBox.top = inst->boundingBox.top + std::numeric_limits<float>::epsilon(); }
		if (inst->boundingBox.back < bsp->boundingBox.back) { regenerate = true; bsp->boundingBox.back = inst->boundingBox.back - std::numeric_limits<float>::epsilon(); }
		if (inst->boundingBox.front > bsp->boundingBox.front) { regenerate = true; bsp->boundingBox.front = inst->boundingBox.front + std::numeric_limits<float>::epsilon(); }

		if (regenerate)
		{
			bsp->instances->push_back(inst);
			BSP_Generate(bsp);
			return;
		}
	}

	//if there are child nodes we will put this instance in one (or both???) of them (call pib and quit out)
	if (bsp->children.child1 != nullptr)
	{
		//if an instance collides with the child bsp, place the instance inside of the child BSP
		if (Bounding_Box_Collision_2D(inst->boundingBox, bsp->children.child1->boundingBox))
		{
			BSP_Place_Into_2D(bsp->children.child1, inst);
		}
		else
		{
			BSP_Place_Into_2D(bsp->children.child2, inst);
		}
		return;
	}
	//else If the current node does not have children, add the instance, increment the meanContentsValue, and check if a split is neccessary
	else
	{
		bsp->instances->push_back(inst);

		//The type of BSP split direction determines the next action
		//and will split the bounding box based on the direction
		if (bsp->type == BSPSplitDirection::leftRight)
		{
			bsp->meanContentsValue += ((inst->boundingBox.right + inst->boundingBox.left) / 2.0f);
		}
		else //topBottom
		{
			bsp->meanContentsValue += ((inst->boundingBox.top + inst->boundingBox.bottom) / 2.0f);
		}

		//Split BSP if neccessary
		if (bsp->instances->size() > bsp->minToSubdivide)
		{
			BSP_Generate_Children_2D(bsp);

			//Place instances into newly created child nodes
			for (auto& i : *bsp->instances)
			{
				BSP_Place_Into_2D(bsp, i);
			}

			//Clear this nodes instances
			bsp->instances->clear();
		}
	}
}
#pragma endregion

