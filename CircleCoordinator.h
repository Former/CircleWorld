#pragma once

#include "CommonTypes.h"
#include "CircleSelector.h"
#include "EngineRules.h"
#include <irrlicht.h>

class CircleCoordinator
{
public:
	struct ObjectColor
	{
		ObjectColor(float a_Red, float a_Green, float a_Blue, float a_Alpha);
		
		float red;
		float green;
		float blue;
		float alpha;
	};
	struct Object
	{
		Object();
		
		CircleEngine::CircleObjectPtr Obj;
		irr::scene::ISceneNode* IrrObject;
		ObjectColor 	Color;
		size_t 			Detal;
	};
	typedef engine_shared_ptr<Object> ObjectPtr;
	
	CircleCoordinator();
	
	void AddObject(const ObjectPtr& a_Object, irr::video::IVideoDriver* a_Driver, irr::scene::ISceneManager* a_SMgr);
	std::vector<ObjectPtr> GetObjects();

	void AddRule(CircleEngine::RulePtr a_Rule);
	void DoStep();
	
	double m_Scale;
	
protected:
	typedef std::map<CircleEngine::COIndex, ObjectPtr> ObjMap;
	ObjMap m_Objects;
	std::vector<CircleEngine::RulePtr> m_Rules;
};
