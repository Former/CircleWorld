#pragma once

#include "Common.h"

struct LOD_Object_Item
{
	LOD_Object_Item(irr::scene::ISceneNode* a_Object, const double& a_Distance);
	
	irr::scene::ISceneNode* m_Object;
	double m_Distance;
};

typedef std::vector<LOD_Object_Item> LOD_Object_Vector;

class LOD_Object
{
public:
	LOD_Object(const std::vector<LOD_Object_Item>& a_Items);
	
	void SetVisibleOneItem(const irr::core::vector3df& a_CameraPosition) const;
	
private:
	std::vector<LOD_Object_Item> m_Items;
};

typedef std::shared_ptr<LOD_Object> LOD_ObjectPtr;
