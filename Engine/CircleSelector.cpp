#include "CircleSelector.h"

namespace CircleEngine
{
	Selector::~Selector()
	{			
	}
	
	////////////////////////////////////////////////////////////

	SequenceSelector::Iterator::Iterator(SequenceSelector* a_Parent)
	{
		m_Index = 0;
		m_Parent = a_Parent;				
	}
			

	CircleObjectPtr SequenceSelector::Iterator::Get() const
	{
		return m_Parent->m_Objects[m_Index];
	}
	
	bool SequenceSelector::Iterator::IsEnd() const
	{
		if (m_Parent->m_Objects.size() == m_Index)
			return true;
		return false;
	}
	
	void SequenceSelector::Iterator::Next()
	{
		if (IsEnd())
			return;
		m_Index++;
	}
		
	SequenceSelector::SequenceSelector()
	{		
	}

	SequenceSelector::~SequenceSelector()
	{		
	}
	
	void SequenceSelector::Add(const CircleObjectPtr& a_Object)
	{
		m_Objects.push_back(a_Object);
	}
	
	void SequenceSelector::Delete(const CircleObjectPtr& a_Object)
	{
		for (size_t i = 0; i < m_Objects.size(); i++)
		{
			if (m_Objects[i].get() == a_Object.get())
			{
				m_Objects.erase(m_Objects.begin() + i);
				break;
			}
		}
	}

	SequenceSelector::Iterator SequenceSelector::Begin()
	{
		return Iterator(this);
	}		
		
	////////////////////////////////////////////////////////////

	PairSelector::Iterator::~Iterator()
	{		
	}
	
	PairSelector::~PairSelector()
	{		
	}
	
	//////////////////////////////////////////////////////////////
	
	SomeToOtherSelector::Iterator::Iterator(SomeToOtherSelector* a_Parent)
	{
		m_SomeIndex = 0;
		m_OtherIndex = 0;
		m_Parent = a_Parent;
	}
	
	SomeToOtherSelector::Iterator::~Iterator()
	{		
	}

	CircleObjectPtr SomeToOtherSelector::Iterator::GetFirst() const
	{
		return m_Parent->m_SomeObjects[m_SomeIndex];
	}
	
	CircleObjectPtr SomeToOtherSelector::Iterator::GetSecond() const
	{
		return m_Parent->m_OtherObjects[m_OtherIndex];
	}	
	
	bool SomeToOtherSelector::Iterator::IsEnd() const
	{
		const size_t size = m_Parent->m_SomeObjects.size();
		return ((m_SomeIndex + 1) > size); 
	}
	
	void SomeToOtherSelector::Iterator::Next()
	{
		if (IsEnd())
			return;

		const size_t size = m_Parent->m_OtherObjects.size();
		m_OtherIndex++;
		if (m_OtherIndex >= size)
		{
			m_SomeIndex++;
			m_OtherIndex = 0;
		}
	}

	SomeToOtherSelector::SomeToOtherSelector()
	{		
	}
	
	SomeToOtherSelector::~SomeToOtherSelector()
	{		
	}
	
	void SomeToOtherSelector::AddSome(const CircleObjectPtr& a_Object)
	{
		m_SomeObjects.push_back(a_Object);
	}
	
	void SomeToOtherSelector::AddOther(const CircleObjectPtr& a_Object)
	{
		m_OtherObjects.push_back(a_Object);
	}
	
	void SomeToOtherSelector::Delete(const CircleObjectPtr& a_Object)
	{
		for (size_t i = 0; i < m_SomeObjects.size(); i++)
		{
			if (m_SomeObjects[i].get() == a_Object.get())
			{
				m_SomeObjects.erase(m_SomeObjects.begin() + i);
				break;
			}
		}
		for (size_t i = 0; i < m_OtherObjects.size(); i++)
		{
			if (m_OtherObjects[i].get() == a_Object.get())
			{
				m_OtherObjects.erase(m_OtherObjects.begin() + i);
				break;
			}
		}
	}

	PairSelector::IteratorPtr SomeToOtherSelector::Begin()
	{
		return IteratorPtr(new Iterator(this));
	}
		
	//////////////////////////////////////////////////////////////
	
	CrossSelector::Iterator::Iterator(CrossSelector* a_Parent)
	{
		m_Index1 = 0;
		m_Index2 = 1;
		m_Parent = a_Parent;
	}

	CrossSelector::Iterator::~Iterator()
	{		
	}

	CircleObjectPtr CrossSelector::Iterator::GetFirst() const
	{
		return m_Parent->m_Objects[m_Index1];
	}
	
	CircleObjectPtr CrossSelector::Iterator::GetSecond() const
	{
		return m_Parent->m_Objects[m_Index2];
	}	
	
	bool CrossSelector::Iterator::IsEnd() const
	{
		const size_t size = m_Parent->m_Objects.size();
		return ((m_Index1 + 1) >= size); 
	}
	
	void CrossSelector::Iterator::Next()
	{
		if (IsEnd())
			return;

		const size_t size = m_Parent->m_Objects.size();
		m_Index2++;
		if (m_Index2 >= size)
		{
			m_Index1++;
			m_Index2 = m_Index1 + 1;
		}
	}
		
	CrossSelector::CrossSelector()
	{		
	}

	CrossSelector::~CrossSelector()
	{		
	}

	void CrossSelector::Add(const CircleObjectPtr& a_Object)
	{
		m_Objects.push_back(a_Object);
	}
	
	void CrossSelector::Delete(const CircleObjectPtr& a_Object)
	{
		for (size_t i = 0; i < m_Objects.size(); i++)
		{
			if (m_Objects[i].get() == a_Object.get())
			{
				m_Objects.erase(m_Objects.begin() + i);
				break;
			}
		}
	}

	PairSelector::IteratorPtr CrossSelector::Begin()
	{
		return IteratorPtr(new Iterator(this));
	}
	
	////////////////////////////////////////////////////////////
	
	CrossNearSelector::Iterator::Iterator(CrossNearSelector* a_Parent)
	{
		m_Index 	= 0;
		m_NearIndex	= 0;
		m_Parent 	= a_Parent;
		if (!IsEnd())
			FindNextFullNear();
	}
	
	CrossNearSelector::Iterator::~Iterator()
	{		
	}

	CircleObjectPtr CrossNearSelector::Iterator::GetFirst() const
	{
		return m_Parent->m_Objects[m_Index];
	}
	
	CircleObjectPtr CrossNearSelector::Iterator::GetSecond() const
	{
		const CONear& near = m_Parent->m_NearObjects[m_Index];
		return near.NearObjects[m_NearIndex];
	}	
	
	bool CrossNearSelector::Iterator::IsEnd() const
	{
		const size_t size = m_Parent->m_Objects.size();
		if (m_Index == size)
			return true;
		
		return false;
	}
	
	void CrossNearSelector::Iterator::Next()
	{
		if (IsEnd())
			return;
		const size_t nearSize = m_Parent->m_NearObjects[m_Index].NearObjects.size();
		
		m_NearIndex++;
		if (m_NearIndex >= nearSize)
		{
			m_Index++;
			m_NearIndex = 0;
			FindNextFullNear();			
		}				
	}
	
	void CrossNearSelector::Iterator::FindNextFullNear()
	{
		size_t nearSize = 0;
		while(!IsEnd() && (nearSize == 0))
		{
			CONear& near = m_Parent->m_NearObjects[m_Index];
			CircleObjectPtr& obj = m_Parent->m_Objects[m_Index];
			if (near.RadiusNearArea < obj->Radius || !m_Parent->IsNear(near.LastCenter, near.RadiusNearArea / 2, obj))
			{
				near.LastCenter 	= obj->Center;
				if (near.RadiusNearArea < obj->Radius)
					near.RadiusNearArea	= obj->Radius * 10.0;
				else
				{
					if (near.NearObjects.size() > m_Parent->m_BestNeighborsCount)
						near.RadiusNearArea = near.RadiusNearArea * 0.8;
					else
						near.RadiusNearArea = near.RadiusNearArea * 1.2;
				}		
				m_Parent->CalculateNear(&near, m_Index);	
			}
			nearSize = near.NearObjects.size();
			if (nearSize == 0)
				m_Index++;
		}
	}
		
	CrossNearSelector::CrossNearSelector(size_t a_BestNeighborsCount)
	{
		m_BestNeighborsCount = a_BestNeighborsCount;
	}

	CrossNearSelector::~CrossNearSelector()
	{		
	}
	
	void CrossNearSelector::Add(const CircleObjectPtr& a_Object)
	{
		m_Objects.push_back(a_Object);
		m_NearObjects.resize(m_Objects.size());
	}
	
	void CrossNearSelector::Delete(const CircleObjectPtr& a_Object)
	{
		for (size_t i = 0; i < m_Objects.size(); i++)
		{
			if (m_Objects[i].get() == a_Object.get())
			{
				m_Objects.erase(m_Objects.begin() + i);
				break;
			}
		}
		m_NearObjects.resize(m_Objects.size());
	}

	PairSelector::IteratorPtr CrossNearSelector::Begin()
	{
		return IteratorPtr(new Iterator(this));
	}
	
		
	CrossNearSelector::CONear::CONear()
	{
		RadiusNearArea 	= 0.0;
	}
	
	bool CrossNearSelector::IsNear(Point a_Center, CoordinateType a_Radius, const CircleObjectPtr& a_Object)
	{
		const Point centerMinusObjCenter = a_Center - a_Object->Center;
		const CoordinateType radiusPlusObjRadius = a_Object->Radius + a_Radius;
		if (centerMinusObjCenter.x > radiusPlusObjRadius || centerMinusObjCenter.x < -radiusPlusObjRadius )
			return false;
		if (centerMinusObjCenter.y > radiusPlusObjRadius || centerMinusObjCenter.y < -radiusPlusObjRadius )
			return false;
		if (centerMinusObjCenter.z > radiusPlusObjRadius || centerMinusObjCenter.z < -radiusPlusObjRadius )
			return false;
		return true;
	}
	
	void CrossNearSelector::CalculateNear(CONear* a_NearData, size_t a_CurIndex)
	{
		a_NearData->NearObjects.resize(0);
		for (size_t i = 0; i < m_Objects.size(); i++)
		{
			if (i == a_CurIndex)
				continue;
			const CircleObjectPtr& obj = m_Objects[i];

			if (IsNear(a_NearData->LastCenter, a_NearData->RadiusNearArea, obj))
				a_NearData->NearObjects.push_back(obj);
		}
	}
}