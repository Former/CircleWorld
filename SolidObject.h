#pragma once

#include "Common.h"
#include "LOD_Object.h"
#include "CircleItem.h"
#include "CreateMeshFromObjectData.h"

class ObjectDrawStrategy;
typedef std::shared_ptr<ObjectDrawStrategy> ObjectDrawStrategyPtr;

class SolidObject
{
public:
	SolidObject(const ObjectDataPtr& a_ObjectData, const ObjectDrawStrategyPtr& a_Strategy, const size_t& a_DivStep, irr::scene::ISceneManager* a_SMgr);
	
	void Draw(const irr::core::vector3df& a_CameraPosition) const;
	void DoStep();

	void SetPosition(const irr::core::vector3df& a_Position);
	irr::core::vector3df GetPosition();	
	
	void SetRotation(const irr::core::vector3df& a_Rotation);
	irr::core::vector3df GetRotation();	
	
private:
	std::vector<LOD_ObjectPtr> m_LOD_OBject;
	ObjectDrawStrategyPtr m_Strategy;
	ObjectDataPtr m_ObjectData;
	irr::core::vector3df m_Rotation;
	irr::core::vector3df m_Position;
	std::vector<StartEndVector> m_StartEndItems;
	IntPoint m_End;
	IntPoint m_Start;
};