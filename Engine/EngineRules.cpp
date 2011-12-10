#include "EngineRules.h"

namespace CircleEngine
{
	Rule::~Rule()
	{
	}
	
	///////////////////////////////////////////////////////////////////////////////

	RuleMove::RuleMove(SequenceSelectorPtr a_Selector, CoordinateType a_Accuracy)
	{
		m_Selector = a_Selector;
		m_Accuracy = a_Accuracy;
	}
	
	RuleMove::~RuleMove()
	{		
	}
		
	void RuleMove::DoStep()
	{
		for (SequenceSelector::Iterator it = m_Selector->Begin(); !it.IsEnd(); it.Next())
		{			
			CircleObjectPtr obj = it.Get();
			
			obj->Center = obj->Center + obj->Velocity * m_Accuracy;
		}
	}

	///////////////////////////////////////////////////////////////////////////////

	RuleGravity::RuleGravity(PairSelectorPtr a_Selector, CoordinateType a_Force, CoordinateType a_Accuracy)
	{
		m_Selector = a_Selector;
		m_Force = a_Force;
		m_Accuracy = a_Accuracy;
	}
		
	RuleGravity::~RuleGravity()
	{			
	}
		
	void RuleGravity::DoStep()
	{
		for (PairSelector::IteratorPtr it = m_Selector->Begin(); !it->IsEnd(); it->Next())
		{			
			const CircleObjectPtr& obj1 = it->GetFirst();
			const CircleObjectPtr& obj2 = it->GetSecond();
			
			CoordinateType dist = obj1->Center.Distance(obj2->Center);
			CoordinateType onedivdist = 0.0;
			if (dist > 0.0)
				onedivdist = 1.0 / dist;

			if (dist > 0.1)
			{
				Point accelerationVector = (obj2->Center - obj1->Center) * onedivdist;
				obj1->Velocity = obj1->Velocity + accelerationVector * onedivdist * onedivdist * m_Force * m_Accuracy * obj2->Weight;
				obj2->Velocity = obj2->Velocity - accelerationVector * onedivdist * onedivdist * m_Force * m_Accuracy * obj1->Weight;
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////

	RuleContact::RuleContact(PairSelectorPtr a_Selector)
	{
		m_Selector = a_Selector;
	}
	
	RuleContact::~RuleContact()
	{		
	}
		
	void RuleContact::DoStep()
	{
		for (PairSelector::IteratorPtr it = m_Selector->Begin(); !it->IsEnd(); it->Next())
		{			
			const CircleObjectPtr& obj1 = it->GetFirst();
			const CircleObjectPtr& obj2 = it->GetSecond();
			
			if (!IsIntersection(obj1, obj2))
				continue;
			
			ResolveContact(obj1, obj2);
		}
	}

	bool RuleContact::IsIntersection(const CircleObjectPtr& a_Object1, const CircleObjectPtr& a_Object2)
	{
		return a_Object1->Center.CheckConnect(a_Object2->Center, a_Object2->Radius + a_Object1->Radius);	
	}
	
	static void ResolveCollision(const CircleObjectPtr& a_Object1, const CircleObjectPtr& a_Object2, const CoordinateType a_NewDistance, const CoordinateType a_Zapas, const bool a_EqualVelocytyAfterCollision)
	{
		Point& centr1 = a_Object1->Center;
		Point& centr2 = a_Object2->Center;
		const CoordinateType weight1 = a_Object1->Weight;
		const CoordinateType weight2 = a_Object2->Weight;
		
		const CoordinateType one_div_summ_weight = 1.0 / (weight1 + weight2);
		Point centrWeight = (centr1 * weight1 + centr2 * weight2) * one_div_summ_weight;
		
		CoordinateType dist = centr1.Distance(centr2);
		CoordinateType onedivdist = 0.0;
		if (dist > 0.0)
			onedivdist = 1.0 / dist;

		// Ñòàëêèâàíèå (çàêîí ñîõðàíåíèÿ èìïóëüñà)
		Point normVector = (centr2 - centr1) * onedivdist;
		
		Point& vel1 = a_Object1->Velocity;
		Point& vel2 = a_Object2->Velocity;
		
		Point vel2Paralell	= normVector * (normVector * vel2);
		Point vel2Perp		= vel2 - vel2Paralell;

		Point vel1Paralell	= normVector * (normVector * vel1);
		Point vel1Perp		= vel1 - vel1Paralell;

		if (a_EqualVelocytyAfterCollision)
		{
			// Объекты после соударения остаються с одинаковой скоростью (они соединены "стержнем")
			Point newVelParalell = (vel1Paralell * weight1 + vel2Paralell * weight2) * one_div_summ_weight;
			vel1 = vel1Perp + newVelParalell;
			vel2 = vel2Perp + newVelParalell;
		}
		else
		{
			// Простое соударение 
			if ((vel1Paralell * normVector) > 0 || (vel2Paralell * normVector) < 0)
			{
				vel1 = vel1Perp + (vel2Paralell * 2.0 * weight2  + vel1Paralell * (weight1 - weight2)) * one_div_summ_weight;
				vel2 = vel2Perp + (vel1Paralell * 2.0 * weight1 + vel2Paralell * (weight2 - weight1)) * one_div_summ_weight;
			}
		}
		
		// Îòäàëåíèå îêðóæíîñòåé äðóã îò äðóãà
		centr1 = normVector * (-(a_NewDistance) * weight2 * one_div_summ_weight * (1.0 + a_Zapas)) + centrWeight;
		centr2 = normVector * (+(a_NewDistance) * weight1 * one_div_summ_weight * (1.0 + a_Zapas)) + centrWeight;
	}
	
	void RuleContact::ResolveContact(const CircleObjectPtr& a_Object1, const CircleObjectPtr& a_Object2)
	{
		ResolveCollision(a_Object1, a_Object2, a_Object1->Radius + a_Object2->Radius, 0.01, false);
	}
	
	/////////////////////////////////////////////////////////////////////////
	
	RuleStrongBar::RuleStrongBar(PairBarSelectorPtr a_Selector)
	{
		m_Selector = a_Selector;
	}
	
	RuleStrongBar::~RuleStrongBar()
	{		
	}
	
	void RuleStrongBar::DoStep()
	{
		for (PairBarSelector::IteratorPtr it = m_Selector->Begin(); !it->IsEnd(); it->Next())
		{			
			const CircleObjectPtr& obj1 = it->GetFirst();
			const CircleObjectPtr& obj2 = it->GetSecond();
			const BarProperties& data = it->GetUserData();
			
			ResolveStrongBar(obj1, obj2, data);
		}
	}
	
	void RuleStrongBar::ResolveStrongBar(const CircleObjectPtr& a_Object1, const CircleObjectPtr& a_Object2, const BarProperties& a_Properties)
	{
		ResolveCollision(a_Object1, a_Object2, a_Properties.Distance, 0.00, true);
	}
}