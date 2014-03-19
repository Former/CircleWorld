#pragma once

#include "Common.h"
#include "SolidObject.h"
#include "IntPoint.h"
#include "CircleItem.h"

class ObjectDrawStrategy
{
public:
	virtual ~ObjectDrawStrategy() {}
	
	virtual irr::video::SColor GetColor(const CircleItem& a_Item, const IntPoint& a_Point) = 0;
	
	virtual irr::video::SMaterial GetMaterial(const CircleItem& a_Item) = 0;
	
	virtual bool AddFace(const IntPoint& a_CurPoint, const IntPoint& a_NearPoint, const size_t& a_DrawStep) = 0;
	
	virtual bool IgnoreFace(const IntPoint& a_CurPoint, const size_t& a_DrawStep) = 0;
};

typedef std::shared_ptr<ObjectDrawStrategy> ObjectDrawStrategyPtr;

typedef std::vector<irr::scene::SMesh*> SMeshVector;

SMeshVector CreateMeshFromObjectData(const CircleVectorZ& a_ObjectData, const ObjectDrawStrategyPtr& a_Strategy, const double& a_Step, const size_t& a_DrawStep = 1, const size_t& a_DivStep = 1024);













