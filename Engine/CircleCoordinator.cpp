#include "CircleCoordinator.h"

namespace CircleEngine
{
	CircleCoordinator::CircleCoordinator()
	{		
	}
	
	void CircleCoordinator::AddObject(const CircleObjectPtr& a_Object)
	{
		m_Objects.push_back(a_Object);
	}
	
	std::vector<CircleObjectPtr> CircleCoordinator::GetObjects()
	{
		return m_Objects;
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