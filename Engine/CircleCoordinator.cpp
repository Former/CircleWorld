#include "CircleCoordinator.h"

namespace CircleEngine
{
	CircleCoordinator::ObjectColor::ObjectColor(float a_Red, float a_Green, float a_Blue)
	{
		red 	= a_Red;
		green 	= a_Green;
		blue 	= a_Blue;
	}	

	CircleCoordinator::Object::Object()
	:	Color(1, 1, 1)
	{
		Detal 	= 10;
	}	
	
	CircleCoordinator::CircleCoordinator()
	{
	}
	
	void CircleCoordinator::AddObject(const ObjectPtr& a_Object)
	{
		m_Objects[a_Object->Obj->GetID()] = a_Object;
	}
	
	std::vector<CircleCoordinator::ObjectPtr> CircleCoordinator::GetObjects()
	{
		std::vector<ObjectPtr> result; 
		
		for (ObjMap::iterator it = m_Objects.begin(); it != m_Objects.end(); it++)
			result.push_back(it->second); 
		
		return result;
	}

	void CircleCoordinator::AddRule(RulePtr a_Rule)
	{
		m_Rules.push_back(a_Rule);
	}
	
	void CircleCoordinator::DoStep()
	{
		for(size_t i = 0; i < m_Rules.size(); i++)
		{
			m_Rules[i]->DoStep();
		}
	}	
}