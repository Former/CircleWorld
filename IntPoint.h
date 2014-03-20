#pragma once

#include "Common.h"
#include "LOD_Object.h"

struct IntPoint
{
	IntPoint();
	IntPoint(const int& a_X, const int& a_Y, const int& a_Z);
	
	int x;
	int y;
	int z;
};
