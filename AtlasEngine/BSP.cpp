#include "pch.h"
#include "BSP.h"
#include <iostream>

void Create_Root_BSP(int maxSubdivisions, int minToSubdivide) {
	//get bounds of the window, in this case its 800 x 800
	int width = 800;
	int height = 800;

	BSP rootBSP{};

	//set topleft and bottom right of the root box
	rootBSP.topLeft = glm::vec2(-1.0f, 1.0f);
	rootBSP.bottomRight = glm::vec2(1.0f, -1.0f);

	//set the floats to be the edges of the window, However these will need to be discerned between x and y values, as they are just floats
	rootBSP.boundingBox.left = -1.0f;
	rootBSP.boundingBox.right = 1.0f;
	rootBSP.boundingBox.top = 1.0f;
	rootBSP.boundingBox.bottom = -1.0f;
	rootBSP.boundingBox.top = 1.0f;
	rootBSP.boundingBox.bottom = -1.0f;
	//fill in struct data
	rootBSP.subdivisionLevel = 0;
	rootBSP.split = false;
	rootBSP.meanContentsValue = 0.0f;
	rootBSP.initialized = true;
	rootBSP.maxSubdivisions = maxSubdivisions;
	rootBSP.minToSubdivide = minToSubdivide;

	//TEMP
	//rootBSP.instances.clear();
}

void Fill_BSP(BSP* bsp, std::vector<Instance*> instances)
{
	if (!bsp->initialized) return;

	//Add the instances to the bsp, do nothing else
	for (auto& i : instances)
	{
		bsp->instances.push_back(i);
	}
}

void Fill_BSP(BSP* bsp, Instance* instance)
{
	if (!bsp->initialized) return;

	//Add the instance to the bsp, do nothing else
	bsp->instances.push_back(instance);
}

void Generate_BSP(BSP* bsp)
{
	//this function is used when it is required to regenerate a bsp. Will recollect all the instances
	//from the children bsps then resitribute them back down into children
	if (!bsp->initialized) return;

	//Take all other instances below this node and add it to this one, then delete all child nodes
	std::vector<Instance*>* childInstances = new std::vector<Instance*>();
	Collect_BSP_Children(bsp, childInstances);

	for (auto& i : bsp->instances)
	{
		childInstances->push_back(i);
	}

	//Remove all instance data in this node and below. Now all instances will be stored in 'childInstances' alone
	Clear_BSP_Node(bsp);

	//Put all the collected instances in the cleared node, then split this node into children as neccessary
	for (auto& i : *childInstances)
	{
		bsp->instances.push_back(i);
	}
	Smart_Distribute_Down_BSP(bsp);

	//cleanup
	delete childInstances;
}

void Smart_Distribute_Down_BSP(BSP* bsp)
{
	//this section sets the mean contents value of the instance based on which 
	//split direction the bsp uses
	//Average the position values of the instances
	if (bsp->type == BSPSplitDirection::leftRight)
	{
		for (auto& i : bsp->instances) 
		{
			//gets the mean of the instance values by getting their center float value 
			//of their bounding box split, in this instance the middle of the left and
			//right bounding boxes
			bsp->meanContentsValue += ((i->boundingBox.right + i->boundingBox.left) / 2.0f);
		}
	}
	else if (bsp->type == BSPSplitDirection::frontBack)
	{
		for (auto& i : bsp->instances)
		{
			bsp->meanContentsValue += ((i->boundingBox.front + i->boundingBox.back) / 2.0f);
		}
	}
	else
	{
		for (auto& i : bsp->instances)
		{
			bsp->meanContentsValue += ((i->boundingBox.top + i->boundingBox.bottom) / 2.0f);
		}
	}

	//Subdivide 
	if (bsp->instances.size() > bsp->minToSubdivide) 
	{
		//create BSP children
		Generate_BSP_Children(bsp);

		//loop through the bsp's instances
		for (auto& i : bsp->instances)
		{
			//if the bsp's child1 collides with the bounding box if the instance
			//add the instance to the child bsp, and removed from parent bsp
			if (Bounding_Box_Collision(i->boundingBox, bsp->children.child1->boundingBox))
			{
				Fill_BSP(bsp->children.child1, i);
			}
			//if the instance does not collide with the child1 bsp, assume the
			//instance collides with the child2 bsp
			else
			{
				Fill_BSP(bsp->children.child2, i);
			}
		}

		bsp->instances.clear();

		//calls distribute down function, which checks the newly
		//created children and their instances to see if they need to
		//be subdivided once again
		Smart_Distribute_Down_BSP(bsp->children.child1);
		Smart_Distribute_Down_BSP(bsp->children.child2);
	}
}

void Collect_BSP_Children(BSP* bsp, std::vector<Instance*>* childInstances)
{
	//This function essentially gets all of the instances in the BSP
	//by loopig recursively
	if (bsp->children.child1 != nullptr)
	{
		//while child1 exits, loop through the child and get all the instances
		//and add them to the childInstances vector
		for (auto& i : bsp->children.child1->instances)
		{
			childInstances->push_back(i);
		}
		//recursively call the function to traves the tree of child bsps,
		//and collect their children, function continues past recursive call
		Collect_BSP_Children(bsp->children.child1, childInstances);

		//do the same for child2 as above
		for (auto& i : bsp->children.child2->instances)
		{
			childInstances->push_back(i);
		}
		Collect_BSP_Children(bsp->children.child2, childInstances);
	}
}

void Init_BSP(BSP* bsp, int maxSubdivisions, int minToSubdivide)
{
	bsp->type = BSPSplitDirection::leftRight;

	bsp->maxSubdivisions = maxSubdivisions;
	bsp->minToSubdivide = minToSubdivide;
	bsp->subdivisionLevel = 0;
	bsp->split = false;
	bsp->meanContentsValue = 0.0f;

	//Remove children and parents if there are any
	Clear_BSP_Node(bsp);

	bsp->boundingBox.back = 0.0f;
	bsp->boundingBox.front = 0.0f;
	bsp->boundingBox.left = 0.0f;
	bsp->boundingBox.right = 0.0f;
	bsp->boundingBox.top = 0.0f;
	bsp->boundingBox.bottom = 0.0f;

	bsp->initialized = true;
}

void Place_Into_BSP(BSP* bsp, Instance* inst)
{
	if (!bsp->initialized) return;

	//If this is the head node and the instance is outside of its bounds, we have to regenerate everything
	if (bsp->subdivisionLevel == 0)
	{
		bool regenerate = false;
		if (inst->boundingBox.left < bsp->boundingBox.left) { regenerate = true; bsp->boundingBox.left = inst->boundingBox.left + std::numeric_limits<float>::epsilon(); }
		if (inst->boundingBox.right > bsp->boundingBox.right) { regenerate = true; bsp->boundingBox.right = inst->boundingBox.right + std::numeric_limits<float>::epsilon(); }
		if (inst->boundingBox.bottom < bsp->boundingBox.bottom) { regenerate = true; bsp->boundingBox.bottom = inst->boundingBox.bottom + std::numeric_limits<float>::epsilon(); }
		if (inst->boundingBox.top > bsp->boundingBox.top) { regenerate = true; bsp->boundingBox.top = inst->boundingBox.top + std::numeric_limits<float>::epsilon(); }
		if (inst->boundingBox.back < bsp->boundingBox.back) { regenerate = true; bsp->boundingBox.back = inst->boundingBox.back + std::numeric_limits<float>::epsilon(); }
		if (inst->boundingBox.front > bsp->boundingBox.front) { regenerate = true; bsp->boundingBox.front = inst->boundingBox.front + std::numeric_limits<float>::epsilon(); }

		if (regenerate)
		{
			Generate_BSP(bsp);
			return;
		}
	}

	//if there are child nodes we will put this instance in one (or both???) of them (call pib and quit out)
	if (bsp->children.child1 != nullptr)
	{
		//if an instance collides with the child bsp, place the instance inside of the child BSP
		if (Bounding_Box_Collision(inst->boundingBox, bsp->children.child1->boundingBox))
		{
			Place_Into_BSP(bsp->children.child1, inst);
		}
		else
		{
			Place_Into_BSP(bsp->children.child2, inst);
		}
		return;
	}
	//else If the current node does not have children, add the instance, increment the meanContentsValue, and check if a split is neccessary
	else
	{
		bsp->instances.push_back(inst);

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

		//Split BSP if neccessary, if the number of instances
		//surpasses the minimum number to subdivide
		if (bsp->instances.size() > bsp->minToSubdivide)
		{
			Generate_BSP_Children(bsp);

			//Place instances into newly created child nodes
			for (auto& i : bsp->instances)
			{
				Place_Into_BSP(bsp, i);
			}

			//Clear this nodes instances
			bsp->instances.clear();
		}
	}
}

void Generate_BSP_Children(BSP* bsp) //Add new children to a node
{
	//split the bounding box along the median values of the instances
	float splitLoc = bsp->meanContentsValue / bsp->instances.size();

	//create child with parent's values as a default
	BSP child1{};
	Init_BSP(&child1, bsp->maxSubdivisions, bsp->minToSubdivide);
	child1.parent = bsp;
	child1.subdivisionLevel = bsp->subdivisionLevel + 1;

	child1.boundingBox.left = bsp->boundingBox.left;
	child1.boundingBox.right = bsp->boundingBox.right;
	child1.boundingBox.front = bsp->boundingBox.front;
	child1.boundingBox.back = bsp->boundingBox.back;
	child1.boundingBox.top = bsp->boundingBox.top;
	child1.boundingBox.bottom = bsp->boundingBox.bottom;

	BSP child2{};
	Init_BSP(&child2, bsp->maxSubdivisions, bsp->minToSubdivide);
	child2.parent = bsp;
	child2.subdivisionLevel = bsp->subdivisionLevel + 1;

	child2.boundingBox.left = bsp->boundingBox.left;
	child2.boundingBox.right = bsp->boundingBox.right;
	child2.boundingBox.front = bsp->boundingBox.front;
	child2.boundingBox.back = bsp->boundingBox.back;
	child2.boundingBox.top = bsp->boundingBox.top;
	child2.boundingBox.bottom = bsp->boundingBox.bottom;

	//depending on the split direction, set the children bsp's values 
	//to the split location and iterate to the next enum value of split
	//direction, the order is LeftRight->FrontBack->TopBottom
	if (bsp->type == BSPSplitDirection::leftRight)
	{
		//if split of parent is LeftRight, set the child's split to be
		//topBottom, which is the next enum in order
		child1.type = BSPSplitDirection::frontBack;
		child1.boundingBox.right = splitLoc;

		child2.type = BSPSplitDirection::frontBack;
		child2.boundingBox.left = splitLoc;
	}
	else if (bsp->type == BSPSplitDirection::frontBack)
	{
		//same as above, only difference is splitting along frontback
		//and setting split sirection enum to topBottom
		child1.type = BSPSplitDirection::topBottom;
		child1.boundingBox.front = splitLoc;

		child2.type = BSPSplitDirection::topBottom;
		child2.boundingBox.back = splitLoc;
	}
	else
	{
		//same as above, only difference is splitting along topbottom
		//and setting split direction back to leftRight, restting the enum
		child1.type = BSPSplitDirection::leftRight;
		child1.boundingBox.top = splitLoc;

		child2.type = BSPSplitDirection::leftRight;
		child2.boundingBox.bottom = splitLoc;
	}

	//bind children to parent
	bsp->children.child1 = &child1;
	bsp->children.child2 = &child2;
}

void Place_Into_BSP2D(BSP* bsp, Instance* instance) {
	if (!bsp->initialized) return;

	if (bsp->children.child1 != nullptr)
	{
		//call Bounding_Box_Collison_2D, which does not account for front/back
		if (Bounding_Box_Collision2D(bsp->boundingBox, bsp->children.child1->boundingBox))
		{
			Place_Into_BSP(bsp->children.child1, instance);
		}
		else
		{
			Place_Into_BSP(bsp->children.child2, instance);
		}
		return;
	}
	else {
		bsp->instances.push_back(instance);

		//The type of BSP determines the next action
		if (bsp->type == BSPSplitDirection::leftRight)
		{
			bsp->meanContentsValue += ((instance->boundingBox.right + instance->boundingBox.left) / 2.0f);

			//Split BSP if neccessary
			if (bsp->instances.size() > bsp->minToSubdivide)
			{
				float splitLoc = bsp->meanContentsValue / bsp->instances.size();

				BSP child1{};
				Init_BSP(&child1, bsp->maxSubdivisions, bsp->minToSubdivide);

				//if type is leftright, change type to topbottom and make bounding box, and do not check for front and back
				child1.type = BSPSplitDirection::topBottom;
				child1.parent = bsp;

				child1.boundingBox.left = bsp->boundingBox.left;
				child1.boundingBox.right = splitLoc;
				//child1.boundingBox.front = bsp->boundingBox.front;
				//child1.boundingBox.back = bsp->boundingBox.back;
				child1.boundingBox.top = bsp->boundingBox.top;
				child1.boundingBox.bottom = bsp->boundingBox.bottom;

				bsp->children.child1 = &child1;

				//2
				BSP child2{};
				Init_BSP(&child2, bsp->maxSubdivisions, bsp->minToSubdivide);

				child2.type = BSPSplitDirection::topBottom;
				child2.parent = bsp;

				child2.boundingBox.left = splitLoc;
				child2.boundingBox.right = bsp->boundingBox.right;
				//child2.boundingBox.front = bsp->boundingBox.front;
				//child2.boundingBox.back = bsp->boundingBox.back;
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
		//Does not check for frontBack because the program functions as a 2d program
		else //topBottom
		{
			bsp->meanContentsValue += ((instance->boundingBox.top + instance->boundingBox.bottom) / 2.0f);

			//Split BSP if neccessary
			if (bsp->instances.size() > bsp->minToSubdivide)
			{
				float splitLoc = bsp->meanContentsValue / bsp->instances.size();

				BSP child1{};
				Init_BSP(&child1, bsp->maxSubdivisions, bsp->minToSubdivide);

				child1.type = BSPSplitDirection::leftRight;
				child1.parent = bsp;

				child1.boundingBox.left = bsp->boundingBox.left;
				child1.boundingBox.right = bsp->boundingBox.right;
				//child1.boundingBox.front = bsp->boundingBox.front;
				//child1.boundingBox.back = bsp->boundingBox.back;
				child1.boundingBox.top = splitLoc;
				child1.boundingBox.bottom = bsp->boundingBox.bottom;

				bsp->children.child1 = &child1;

				//2
				BSP child2{};
				Init_BSP(&child2, bsp->maxSubdivisions, bsp->minToSubdivide);

				child2.type = BSPSplitDirection::leftRight;
				child2.parent = bsp;

				child2.boundingBox.left = bsp->boundingBox.left;
				child2.boundingBox.right = bsp->boundingBox.right;
				//child2.boundingBox.front = bsp->boundingBox.front;
				//child2.boundingBox.back = bsp->boundingBox.back;
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

	//Find children
	if (bsp->children.child1 != nullptr)
	{
		Clear_BSP_Node(bsp->children.child1);
		Clear_BSP_Node(bsp->children.child2);

		//Delete children
		delete bsp->children.child1;
		delete bsp->children.child2;

		bsp->children.child1 = nullptr;
		bsp->children.child2 = nullptr;
	}
	//Remove instances in node
	bsp->instances.clear();
}

