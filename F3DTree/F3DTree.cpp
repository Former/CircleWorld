#include "F3DTree.h"

#ifdef _DEBUG
#define ASSERT(x) do{ if(!(x)) { __asm int 3 } }while(0)
#else
#define ASSERT(x) do{ }while(0)
#endif
	
const IntPoint[8] = 
{
	IntPoint(0,0,0),
	IntPoint(1,0,0),
	IntPoint(0,1,0),
	IntPoint(1,1,0),
	IntPoint(0,0,1),
	IntPoint(1,0,1),
	IntPoint(0,1,1),
	IntPoint(1,1,1),
} g_IndexToPointer;

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

IntPoint ConvertPoiterToNewIndex(const IntPoint& a_CurPoint, const size& a_CurIndex, const size& a_NewIndex)
{
	const size_t sdvig = a_NewIndex - a_CurIndex;
	return IntPoint(a_CurPoint.x >> sdvig, _CurPoint.y >> sdvig, _CurPoint.z >> sdvig);	
}