#include "F3DTree/F3DTree.h"
	
static const IntPoint g_IndexToPointer[8] = 
{
	IntPoint(0,0,0),
	IntPoint(1,0,0),
	IntPoint(0,1,0),
	IntPoint(1,1,0),
	IntPoint(0,0,1),
	IntPoint(1,0,1),
	IntPoint(0,1,1),
	IntPoint(1,1,1),
};

size_t PoiterToIndex(const IntPoint& a_CurPoint)
{
	ASSERT(a_CurPoint >= IntPoint(0, 0, 0));
	ASSERT(a_CurPoint <= IntPoint(1, 1, 1));
	
	return a_CurPoint.z * 4 + a_CurPoint.y * 2 + a_CurPoint.x;
}

const IntPoint& IndexToPoiter(const size_t& a_CurIndex)
{
	ASSERT(a_CurIndex <= (sizeof(g_IndexToPointer) / sizeof(g_IndexToPointer[0])));
	return g_IndexToPointer[a_CurIndex];
}

IntPoint ConvertPoiterToNewIndex(const IntPoint& a_CurPoint, const size_t& a_CurIndex, const size_t& a_NewIndex)
{
	const size_t sdvig = a_NewIndex - a_CurIndex;
	ASSERT(sdvig < 32);
	return IntPoint(a_CurPoint.x >> sdvig, a_CurPoint.y >> sdvig, a_CurPoint.z >> sdvig);	
}