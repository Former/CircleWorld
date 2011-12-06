#pragma once

#include "CommonTypes.h"
#include "CircleSelector.h"

namespace CircleEngine
{
	class Rule
	{
	public:
		virtual ~Rule();
		
		virtual void DoStep() = 0;
	};
	typedef engine_shared_ptr<Rule> RulePtr;
	
	/////////////////////////////////////////////////////////////////////
	
	class RuleMove : public Rule
	{
	public:
		RuleMove(SequenceSelectorPtr a_Selector, CoordinateType a_Accuracy);
		virtual ~RuleMove();
		
		virtual void DoStep() override;
		
	protected:
		SequenceSelectorPtr m_Selector;
		CoordinateType m_Accuracy;
	};

	/////////////////////////////////////////////////////////////////////

	class RuleGravity : public Rule
	{
	public:
		RuleGravity(PairSelectorPtr a_Selector, CoordinateType a_Force, CoordinateType a_Accuracy);
		virtual ~RuleGravity();
		
		virtual void DoStep() override;
		
	protected:
		PairSelectorPtr m_Selector;
		CoordinateType m_Force;
		CoordinateType m_Accuracy;
	};

	/////////////////////////////////////////////////////////////////////

	class RuleContact : public Rule
	{
	public:
		RuleContact(PairSelectorPtr a_Selector);
		virtual ~RuleContact();
		
		virtual void DoStep() override;
		
	protected:
		static bool	IsIntersection(const CircleObjectPtr& a_Object1, const CircleObjectPtr& a_Object2);
		static void ResolveContact(const CircleObjectPtr& a_Object1, const CircleObjectPtr& a_Object2, bool a_IsObject1Fixed);
		
		PairSelectorPtr m_Selector;
	};
}