#pragma once

#include <irrlicht.h>
#include <vector>

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

typedef std::vector<CircleItem> CircleVectorX;
typedef std::vector<CircleVectorX> CircleVectorY;
typedef std::vector<CircleVectorY> CircleVectorZ;

irr::scene::SMesh* CreateMeshFromObjectData(const CircleVectorZ& a_ObjectData, const double& a_Step, const size_t& a_DrawStep = 1);