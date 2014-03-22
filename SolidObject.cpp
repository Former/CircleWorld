#include "SolidObject.h"

struct LOD_Settings
{
	double m_Distance;
	size_t m_DrawStep;
};

const LOD_Settings settings[] =
{
	{8000, 1},
	{16000, 4},
	{32000, 16},
//	{100000, 32},
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
	
	m_End = a_Strategy->GetEndPoint();
	m_Start = a_Strategy->GetStartPoint();

	for (size_t i = 0; i < ARRAY_SIZE(settings); ++i)
	{
		const LOD_Settings& set_item = settings[i];
		m_StartEndItems.push_back(MakeStartEndItems(m_Start, m_End, set_item.m_DrawStep, a_DivStep));
	}
	
	size_t num_of_cpu = a_Strategy->GetThreadCount();
	std::vector<std::vector<StartEndVector>> items;
	for (size_t i = 0; i < m_StartEndItems.size(); ++i)
	{
		std::vector<StartEndVector> se_vector = MakeStartEndVectorByGroup(m_StartEndItems[i], num_of_cpu);
		items.resize(se_vector.size());
		for (size_t j = 0; j < se_vector.size(); ++j)
		{
			const StartEndVector& cur_se = se_vector[j];
			items[j].push_back(cur_se);
		}
	}

	std::vector<FutureMeshDrawStepsVector> worker_results;
	for (size_t j = 0; j < items.size(); ++j)
	{
		const std::vector<StartEndVector>& item = items[j];
		StartEndStepVector steps;
		for (size_t i = 0; i < item.size(); ++i)
		{
			const LOD_Settings& set_item = settings[i];
			const StartEndVector& se_vector = item[i];
			steps.push_back(StartEndStepItem(se_vector, set_item.m_DrawStep));
		}
		
		double step = a_Strategy->GetStep();
		worker_results.push_back(std::async(std::launch::async, CreateMeshFromObjectDataWorker, steps, a_Strategy, step, m_End));
	}
	
	for (size_t j = 0; j < worker_results.size(); ++j)
	{
		const SMeshDrawStepsVector& cur_result = worker_results[j].get();
		for (size_t i = 0; i < cur_result.size(); ++i)
		{
			const SMeshVector& cur_mesh_vector = cur_result[i];
			LOD_Object_Vector cur_lod_object_vector;
			for (size_t k = 0; k < cur_mesh_vector.size(); ++k)
			{
				const LOD_Settings& set_item = settings[k];
				irr::scene::SMesh* mesh = cur_mesh_vector[k];
				irr::scene::ISceneNode* object_node = MakeNodeFromMesh(mesh, a_SMgr);
				
				LOD_Object_Item new_item(object_node, set_item.m_Distance);
				cur_lod_object_vector.push_back(new_item);
			}
			
			LOD_ObjectPtr lod_item(new LOD_Object(cur_lod_object_vector));
			m_LOD_OBject.push_back(lod_item);
		}		
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

	for (size_t z = m_Start.z;  z < m_End.z; ++z)
	{
		for (size_t y = m_Start.y;  y < m_End.y; ++y)
		{
			for (size_t x = m_Start.x;  x < m_End.x; ++x)
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
