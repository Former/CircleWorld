#pragma once 

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
		tpWater,
		tpCount,
	};
	
	Type m_Type;
};

typedef std::vector<CircleItem> CircleVectorX;
typedef std::vector<CircleVectorX> CircleVectorY;
typedef std::vector<CircleVectorY> CircleVectorZ;

typedef std::shared_ptr<CircleVectorZ> ObjectDataPtr;