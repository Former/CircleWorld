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
	
	class RuleFriction : public Rule
	{
	public:
		RuleFriction(PairSelectorPtr a_Selector, CoordinateType a_Force);
		virtual ~RuleFriction();
		
		virtual void DoStep() override;
		
	protected:
		PairSelectorPtr m_Selector;
		CoordinateType m_Force;
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
		static void ResolveContact(const CircleObjectPtr& a_Object1, const CircleObjectPtr& a_Object2);
		
		PairSelectorPtr m_Selector;
	};

	/////////////////////////////////////////////////////////////////////
	struct BarProperties
	{
		CoordinateType Distance;
	};	
	typedef PairUserSelector<BarProperties> PairBarSelector;
	typedef engine_shared_ptr< PairUserSelector<BarProperties> > PairBarSelectorPtr;
	
	class RuleStrongBar : public Rule
	{
	public:
		RuleStrongBar(PairBarSelectorPtr a_Selector);
		virtual ~RuleStrongBar();
		
		virtual void DoStep() override;
		
	protected:
		static void ResolveStrongBar(const CircleObjectPtr& a_Object1, const CircleObjectPtr& a_Object2, const BarProperties& a_Properties);

		PairBarSelectorPtr m_Selector;
	};
	
	/////////////////////////////////////////////////////////////////////
	
	class RuleStrongDistance : public Rule
	{
	public:
		RuleStrongDistance(PairBarSelectorPtr a_Selector);
		virtual ~RuleStrongDistance();
		
		virtual void DoStep() override;
		
	protected:
		static void ResolveStrongDistance(const CircleObjectPtr& a_Object1, const CircleObjectPtr& a_Object2, const BarProperties& a_Properties);

		PairBarSelectorPtr m_Selector;
	};

}