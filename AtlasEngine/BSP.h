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

struct BSP
{
	int maxSubdivisions;
	int minToSubdivide;

	int numInstances;
	Instance* instances;

	int numChildren;
	BSP* children;

	BSP* parent;

	BoundingBox boundingBox;

	//array of booleans top / left / front


};

//Root node initialization
void Init_BSP(BoundingBox boundingBox, int maxSubdivisions, int minToSubdivide);

//Check if it should subdivide
//-> left/right first

//
void Subdivide();

