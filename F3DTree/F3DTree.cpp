#include "F3DTree/F3DTree.h"
	
static const TreePoint g_IndexToPointer[8] = 
{
	TreePoint(0,0,0),
	TreePoint(1,0,0),
	TreePoint(0,1,0),
	TreePoint(1,1,0),
	TreePoint(0,0,1),
	TreePoint(1,0,1),
	TreePoint(0,1,1),
	TreePoint(1,1,1),
};

size_t PoiterToIndex(const TreePoint& a_CurPoint)
{
	ASSERT(a_CurPoint >= TreePoint(0, 0, 0));
	ASSERT(a_CurPoint <= TreePoint(1, 1, 1));
	
	return a_CurPoint.z * 4 + a_CurPoint.y * 2 + a_CurPoint.x;
}

const TreePoint& IndexToPoiter(const size_t& a_CurIndex)
{
	ASSERT(a_CurIndex <= (sizeof(g_IndexToPointer) / sizeof(g_IndexToPointer[0])));
	return g_IndexToPointer[a_CurIndex];
}

TreePoint ConvertPoiterToNewIndex(const TreePoint& a_CurPoint, const size_t& a_CurIndex, const size_t& a_NewIndex)
{
	const size_t sdvig = a_NewIndex - a_CurIndex;
	ASSERT(sdvig < 32);
	return TreePoint(a_CurPoint.x >> sdvig, a_CurPoint.y >> sdvig, a_CurPoint.z >> sdvig);	
}