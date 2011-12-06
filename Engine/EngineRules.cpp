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
			CircleObjectPair pair = it->Get();
			CircleObjectPtr& obj1 = pair.first;
			CircleObjectPtr& obj2 = pair.second;
			
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
			CircleObjectPair pair = it->Get();
			CircleObjectPtr& obj1 = pair.first;
			CircleObjectPtr& obj2 = pair.second;
			
			if (!IsIntersection(obj1, obj2))
				continue;
			
			if (!obj1->IsFixed && !obj2->IsFixed)
			{
				ResolveContact(obj1, obj2, false);
				continue;
			}	
			
			if (obj2->IsFixed && obj1->IsFixed)
				continue;
			
			if (obj2->IsFixed)
				ResolveContact(obj2, obj1, true);
			else
				ResolveContact(obj1, obj2, true);
		}
	}

	bool RuleContact::IsIntersection(const CircleObjectPtr& a_Object1, const CircleObjectPtr& a_Object2)
	{
		return a_Object1->Center.CheckConnect(a_Object2->Center, a_Object2->Radius + a_Object1->Radius);	
	}
		
	void RuleContact::ResolveContact(const CircleObjectPtr& a_Object1, const CircleObjectPtr& a_Object2, bool a_IsObject1Fixed)
	{
		const CoordinateType zapas = 0.01;
		
		Point& centr1 = a_Object1->Center;
		Point& centr2 = a_Object2->Center;
		const CoordinateType weight1 = a_Object1->Weight;
		const CoordinateType weight2 = a_Object1->Weight;
		
		Point centrWeight = (centr1 * weight1 + centr2 * weight2) / (weight1 + weight2);
		
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

		if (a_IsObject1Fixed)
		{
			if ((vel2Paralell * normVector) < 0)
				vel2 = vel2Perp - vel2Paralell;
			else
				vel2 = vel2Perp + vel2Paralell;
			centr2 = normVector * ((a_Object2->Radius + a_Object1->Radius) * (1.0 + zapas)) + centr1;		
			return;
		}

		Point vel1Paralell	= normVector * (normVector * vel1);
		Point vel1Perp		= vel1 - vel1Paralell;

		if ((vel1Paralell * normVector) > 0 || (vel2Paralell * normVector) < 0)
		{
			vel1 = vel1Perp + vel2Paralell;
			vel2 = vel2Perp + vel1Paralell;
		}
		
		// Îòäàëåíèå îêðóæíîñòåé äðóã îò äðóãà
		centr1 = normVector * (-a_Object1->Radius * (1.0 + zapas)) + centrWeight;
		centr2 = normVector * (+a_Object2->Radius * (1.0 + zapas)) + centrWeight;
	}
}