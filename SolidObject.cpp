#include "SolidObject.h"

struct LOD_Settings
{
	double m_Distance;
	size_t m_DrawStep;
};

const LOD_Settings settings[] =
{
	{10000, 1},
	{30000, 4},
	{60000, 16},
	{100000, 64},
};

static irr::scene::ISceneNode* MakeNodeFromMesh(irr::scene::SMesh* a_Mesh, irr::scene::ISceneManager* a_SMgr)
{
	irr::scene::ISceneNode* object_node = a_SMgr->addMeshSceneNode(a_Mesh);
	if (object_node)
	{
		object_node->setDebugDataVisible(irr::scene::EDS_BBOX);
		object_node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	}

	return object_node;
}

////////////////////////////////////////////////////////////

SolidObject::SolidObject(const ObjectDataPtr& a_ObjectData, const ObjectDrawStrategyPtr& a_Strategy, const size_t& a_DivStep, irr::scene::ISceneManager* a_SMgr)
{
	m_ObjectData = a_ObjectData;
	m_Strategy = a_Strategy;
	
	std::vector<LOD_Object_Vector> lod_objects_vectors;
	for (size_t i = 0; i < ARRAY_SIZE(settings); ++i)
	{

		const LOD_Settings& item = settings[i];
		SMeshVector meshs = CreateMeshFromObjectData(*m_ObjectData, m_Strategy, 50.0, item.m_DrawStep, a_DivStep);

		lod_objects_vectors.resize(meshs.size());

		for (size_t j = 0; j < meshs.size(); ++j)
		{
			irr::scene::SMesh* mesh = meshs[j];
			irr::scene::ISceneNode* object_node = MakeNodeFromMesh(mesh, a_SMgr);

			LOD_Object_Item new_item(object_node, item.m_Distance);
			lod_objects_vectors[j].push_back(new_item);
		}
	}

	for (size_t i = 0; i < lod_objects_vectors.size(); ++i)
	{
		LOD_ObjectPtr lod_item(new LOD_Object(lod_objects_vectors[i]));

		m_LOD_OBject.push_back(lod_item);
	}

}

void SolidObject::Draw(const irr::core::vector3df& a_CameraPosition) const
{
	for (size_t i = 0; i < m_LOD_OBject.size(); ++i)
	{
		const LOD_ObjectPtr& item = m_LOD_OBject[i];
		item->SetVisibleOneItem(a_CameraPosition);
	}
}

irr::core::vector3df SolidObject::GetRotation()
{
	return m_Rotation;
}

void SolidObject::SetRotation(const irr::core::vector3df& a_Rotation)
{
	m_Rotation = a_Rotation;
	for (size_t i = 0; i < m_LOD_OBject.size(); ++i)
	{
		const LOD_ObjectPtr& item = m_LOD_OBject[i];
		item->SetRotation(a_Rotation);
	}
}

irr::core::vector3df SolidObject::GetPosition()
{
	return m_Position;
}

void SolidObject::SetPosition(const irr::core::vector3df& a_Position)
{
	m_Position = a_Position;
	for (size_t i = 0; i < m_LOD_OBject.size(); ++i)
	{
		const LOD_ObjectPtr& item = m_LOD_OBject[i];
		item->SetPosition(a_Position);
	}
}

void SolidObject::DoStep()
{
	CircleVectorZ& object_data = *m_ObjectData;
	const size_t max_z = int(object_data.size());
	const size_t max_y = int(object_data[0].size());
	const size_t max_x = int(object_data[0][0].size());
	IntPoint a_NearPoint(0, 0, 0);

	for (size_t z = a_NearPoint.z;  z < max_z; ++z)
	{
		for (size_t y = a_NearPoint.y;  y < max_y; ++y)
		{
			for (size_t x = a_NearPoint.x;  x < max_x; ++x)
			{
				CircleItem& item = object_data[z][y][x];
				if (item.m_Type == CircleItem::tpNone)
					item.m_Type = CircleItem::tpSolid;
				else if (item.m_Type == CircleItem::tpSolid)
					item.m_Type = CircleItem::tpNone;
			}
		}
	}
}
