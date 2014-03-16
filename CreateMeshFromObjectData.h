#pragma once

#include "Common.h"

struct CircleItem
{
	CircleItem()
	{
		m_Type = tpNone;
	}
	
	enum Type
	{
		tpNone = 0,
		tpSolid,
		tpCount,
	};
	
	Type m_Type;
	irr::video::SColor m_Color;
};

struct Point
{
	Point(const int& a_X, const int& a_Y, const int& a_Z);
	
	int x;
	int y;
	int z;
};

class ObjectCreationStrategy
{
public:
	virtual ~ObjectCreationStrategy() {}
	
	virtual irr::video::SColor GetColor(const CircleItem& a_Item, const Point& a_Point) = 0;
	
	virtual irr::video::SMaterial GetMaterial(const CircleItem& a_Item) = 0;
};

typedef std::shared_ptr<ObjectCreationStrategy> ObjectCreationStrategyPtr;

typedef std::vector<CircleItem> CircleVectorX;
typedef std::vector<CircleVectorX> CircleVectorY;
typedef std::vector<CircleVectorY> CircleVectorZ;

typedef std::vector<irr::scene::SMesh*> SMeshVector;

SMeshVector CreateMeshFromObjectData(const CircleVectorZ& a_ObjectData, const ObjectCreationStrategyPtr& a_Strategy, const double& a_Step, const size_t& a_DrawStep = 1, const size_t& a_DivStep = 1024);













