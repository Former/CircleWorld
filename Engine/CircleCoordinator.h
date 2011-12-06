#pragma once

#include "CommonTypes.h"
#include "CircleSelector.h"
#include "EngineRules.h"

namespace CircleEngine
{
	class CircleCoordinator
	{
	public:		
		CircleCoordinator();
		
		void AddObject(const CircleObjectPtr& a_Object);
		std::vector<CircleObjectPtr> GetObjects();

		void AddRule(RulePtr a_Rule);
		void DoStep();
		
	protected:
		std::vector<CircleObjectPtr> m_Objects;
		std::vector<RulePtr> m_Rules;
	};
}