#pragma once

#include "CommonTypes.h"
#include "CircleSelector.h"
#include "EngineRules.h"
#include "luascript/luascript.h"

namespace CircleEngine
{
	class CircleCoordinator
	{
	public:
		struct ObjectColor
		{
			ObjectColor(float a_Red, float a_Green, float a_Blue);
			
			float red;
			float green;
			float blue;
		};
		struct Object
		{
			Object();
			
			CircleObjectPtr Obj;
			ObjectColor 	Color;
			size_t 			Detal;
		};
		typedef engine_shared_ptr<Object> ObjectPtr;
		
		CircleCoordinator();
		
		void AddObject(const ObjectPtr& a_Object);
		std::vector<ObjectPtr> GetObjects();

		void AddRule(RulePtr a_Rule);
		void DoStep();
		
	protected:
		typedef std::map<COIndex, ObjectPtr> ObjMap;
		ObjMap m_Objects;
		std::vector<RulePtr> m_Rules;
	};
}