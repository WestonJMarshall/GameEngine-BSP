#include "pch.h"
#include "BSP.h"
#include <iostream>

BSP* Create_Root_BSP(int maxSubdivisions, int minToSubdivide) {
	//get bounds of the window, in this case its 800 x 800
	int width = 800;
	int height = 800;

	BSP* rootBSP{};

	//set topleft and bottom right of the root box
	rootBSP->topLeft = glm::vec2(-1.0f, 1.0f);
	rootBSP->bottomRight = glm::vec2(1.0f, -1.0f);

	//set the floats to be the edges of the window, However these will need to be discerned between x and y values, as they are just floats
	rootBSP->boundingBox.left = -1.0f;
	rootBSP->boundingBox.right = 1.0f;
	rootBSP->boundingBox.top = 1.0f;
	rootBSP->boundingBox.bottom = -1.0f;
	//fill in struct data
	rootBSP->subdivisionLevel = 0;
	rootBSP->split = false;
	rootBSP->meanContentsValue = 0.0f;
	rootBSP->initialized = true;
	rootBSP->maxSubdivisions = maxSubdivisions;
	rootBSP->minToSubdivide = minToSubdivide;

	//TEMP
	rootBSP->instances.clear();

	//returns root node
	return rootBSP;
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
	if (!bsp->initialized) return;

	//Take all other instances below this node and add it to this one, then delete all child nodes
	std::vector<Instance*>* childInstances;
	Collect_BSP_Children(bsp, childInstances);

	for (auto& i : bsp->instances)
	{
		childInstances->push_back(i);
	}

	Clear_BSP_Node(bsp);

	//split this node into children as neccessary

	for (auto& i : *childInstances)
	{
		bsp->instances.push_back(i);
	}
	Smart_Distribute_Down_BSP(bsp);
}

void Smart_Distribute_Down_BSP(BSP* bsp)
{
	//Average the values in this BSP, then split it if neccessary
	if (bsp->type == leftRight)
	{
		for (auto& i : bsp->instances) //Average the values of the instances
		{
			bsp->meanContentsValue += ((i->boundingBox.right + i->boundingBox.left) / 2.0f);
		}
		if (bsp->instances.size() > bsp->minToSubdivide) //Subdivide
		{
			Generate_BSP_Children(bsp);

			for (auto& i : bsp->instances)
			{
				if (Bounding_Box_Collision(i->boundingBox, bsp->children.child1->boundingBox))
				{
					Fill_BSP(bsp->children.child1, i);
				}
				else
				{
					Fill_BSP(bsp->children.child2, i);
				}
			}

			Smart_Distribute_Down_BSP(bsp->children.child1);
			Smart_Distribute_Down_BSP(bsp->children.child2);
		}
	}
	else if (bsp->type == frontBack)
	{
		for (auto& i : bsp->instances)
		{
			bsp->meanContentsValue += ((i->boundingBox.front + i->boundingBox.back) / 2.0f);
		}
		if (bsp->instances.size() > bsp->minToSubdivide)
		{
			Generate_BSP_Children(bsp);

			for (auto& i : bsp->instances)
			{
				if (Bounding_Box_Collision(i->boundingBox, bsp->children.child1->boundingBox))
				{
					Fill_BSP(bsp->children.child1, i);
				}
				else
				{
					Fill_BSP(bsp->children.child2, i);
				}
			}

			Smart_Distribute_Down_BSP(bsp->children.child1);
			Smart_Distribute_Down_BSP(bsp->children.child2);
		}
	}
	else
	{
		for (auto& i : bsp->instances)
		{
			bsp->meanContentsValue += ((i->boundingBox.top + i->boundingBox.bottom) / 2.0f);
		}
		if (bsp->instances.size() > bsp->minToSubdivide)
		{
			Generate_BSP_Children(bsp);

			for (auto& i : bsp->instances)
			{
				if (Bounding_Box_Collision(i->boundingBox, bsp->children.child1->boundingBox))
				{
					Fill_BSP(bsp->children.child1, i);
				}
				else
				{
					Fill_BSP(bsp->children.child2, i);
				}
			}

			Smart_Distribute_Down_BSP(bsp->children.child1);
			Smart_Distribute_Down_BSP(bsp->children.child2);
		}
	}
}

void Collect_BSP_Children(BSP* bsp, std::vector<Instance*>* childInstances)
{
	if (bsp->children.child1 != nullptr)
	{
		for (auto& i : bsp->children.child1->instances)
		{
			childInstances->push_back(i);
		}
		Collect_BSP_Children(bsp->children.child1, childInstances);

		for (auto& i : bsp->children.child2->instances)
		{
			childInstances->push_back(i);
		}
		Collect_BSP_Children(bsp->children.child2, childInstances);
	}
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
				Generate_BSP_Children(bsp);

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
				Generate_BSP_Children(bsp);

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
				Generate_BSP_Children(bsp);

				//Place instances into newly created child nodes
				for (auto& i : bsp->instances)
				{
					Place_Into_BSP(bsp, i);
				}
			}
		}
	}
}

void Generate_BSP_Children(BSP* bsp)
{
	if (bsp->type == leftRight)
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
	}
	else if (bsp->type == frontBack)
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
	}
	else
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
	}
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
		if (bsp->type == leftRight)
		{
			bsp->meanContentsValue += ((instance->boundingBox.right + instance->boundingBox.left) / 2.0f);

			//Split BSP if neccessary
			if (bsp->instances.size() > bsp->minToSubdivide)
			{
				float splitLoc = bsp->meanContentsValue / bsp->instances.size();

				BSP child1{};
				Init_BSP(&child1, bsp->maxSubdivisions, bsp->minToSubdivide);

				//if type is leftright, change type to topbottom and make bounding box, and do not check for front and back
				child1.type = topBottom;
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

				child2.type = topBottom;
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

				child1.type = leftRight;
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

				child2.type = leftRight;
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

	//Remove all instances from this node downwards, then delete all of this node's children

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
	bsp->instances.clear();
}

