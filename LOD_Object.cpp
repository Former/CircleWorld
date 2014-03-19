#include "LOD_Object.h"

LOD_Object_Item::LOD_Object_Item(irr::scene::ISceneNode* a_Object, const double& a_Distance)
{
	m_Object = a_Object;
	m_Distance = a_Distance;
}

struct LOD_Object_Item_Sort_Pred
{
	bool operator()(const LOD_Object_Item& a_Item1, const LOD_Object_Item& a_Item2)
	{
		return (a_Item1.m_Distance < a_Item2.m_Distance);
	}
};

LOD_Object::LOD_Object(const std::vector<LOD_Object_Item>& a_Items)
{
	m_Items = a_Items;	
	std::sort(m_Items.begin(), m_Items.end(), LOD_Object_Item_Sort_Pred()); 
}

#define FROM_GRADUS_TO_RADIAN 0,017453293 // 2 * pi / 360

static double CalcDistance2(irr::scene::ISceneNode* a_Object, const irr::core::vector3df& a_CameraPosition)
{
	irr::core::matrix4 mat;
	mat.setRotationDegrees(a_Object->getRotation());

	irr::core::vector3df bb_pos = a_Object->getBoundingBox().getCenter();
	mat.rotateVect(bb_pos);
	irr::core::vector3df obj_pos = bb_pos + a_Object->getPosition();
	
	return (obj_pos - a_CameraPosition).getLengthSQ();
}

void LOD_Object::SetVisibleOneItem(const irr::core::vector3df& a_CameraPosition) const 
{
	if (m_Items.empty())
		return;
	
	bool is_one_visible = false;
	for (size_t i = 0; i < m_Items.size(); ++i)
	{
		const LOD_Object_Item& item = m_Items[i];
		if (!is_one_visible && CalcDistance2(item.m_Object, a_CameraPosition) < item.m_Distance * item.m_Distance)
		{
			item.m_Object->setVisible(true);
			is_one_visible = true;
			continue;
		}

		item.m_Object->setVisible(false);
	}
	
	if (!is_one_visible)
		m_Items.back().m_Object->setVisible(true);
}

void LOD_Object::SetRotation(const irr::core::vector3df& a_Rotation)
{
	for (size_t i = 0; i < m_Items.size(); ++i)
	{
		const LOD_Object_Item& item = m_Items[i];

		item.m_Object->setRotation(a_Rotation);
	}
	
	m_Rotation = a_Rotation;
}

irr::core::vector3df LOD_Object::GetRotation()
{
	return m_Rotation;
}
