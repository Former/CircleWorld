#include "SolidObject.h"

struct LOD_Settings
{
	double m_Distance;
	size_t m_DrawStep;
};

const LOD_Settings settings[] =
{
	{5000, 1},
	{20000, 4},
	{40000, 16},
	{80000, 64},
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

SolidObject::SolidObject(const CircleVectorZ& a_ObjectData, const ObjectDrawStrategyPtr& a_Strategy, const size_t& a_DivStep, irr::scene::ISceneManager* a_SMgr)
{
	m_ObjectData = a_ObjectData;
	m_Strategy = a_Strategy;
	
	std::vector<LOD_Object_Vector> lod_objects_vectors;
	for (size_t i = 0; i < ARRAY_SIZE(settings); ++i)
	{

		const LOD_Settings& item = settings[i];
		SMeshVector meshs = CreateMeshFromObjectData(m_ObjectData, m_Strategy, 50.0, item.m_DrawStep, a_DivStep);

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
