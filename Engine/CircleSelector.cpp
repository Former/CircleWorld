#include "CircleSelector.h"

namespace CircleEngine
{
	virtual Selector::~Selector()
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
	
	void SequenceSelector::Add(const CircleObjectPtr& a_Object)
	{
		m_Objects.push_back(a_Object);
	}
	
	void SequenceSelector::Delete(const Iterator& a_Iterator)
	{
		m_Objects.erase(m_Objects.begin() + a_Iterator.m_Index);
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

	Iterator SequenceSelector::Begin()
	{
		return Iterator(this);
	}		
		
	////////////////////////////////////////////////////////////
		
	CrossSelector::Iterator::Iterator(CrossSelector* a_Parent)
	{
		m_Index1 = 0;
		m_Index2 = 1;
		m_Parent = a_Parent;
	}

	CircleObjectPair CrossSelector::Iterator::Get() const
	{
		return CircleObjectPair(m_Parent->m_Objects[m_Index1], m_Parent->m_Objects[m_Index2]);
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

	void CrossSelector::Add(const CircleObjectPtr& a_Object)
	{
		m_Objects.push_back(a_Object);
	}
	
	void CrossSelector::Delete(const Iterator& a_Iterator)
	{
		m_Objects.erase(m_Objects.begin() + a_Iterator.m_Index1 * size + a_Iterator.m_Index2);
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

	Iterator CrossSelector::Begin()
	{
		return Iterator(this);
	}
	
	////////////////////////////////////////////////////////////
	
	CrossNearSelector::Iterator::Iterator(CrossNearSelector* a_Parent)
	{
		m_Index 	= 0;
		m_NearIndex	= 0;
		m_Parent 	= 0;
		if (!IsEnd())
			FindNextFullNear();
	}

	CircleObjectPair CrossNearSelector::Iterator::Get() const
	{
		const CONear& near = m_Parent->m_NearObjects[m_Index];
		return CircleObjectPair(m_Parent->m_Objects[m_Index], near.NearObjects[m_NearIndex]);
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
		const size_t nearSize = m_Parent->m_NearObjects.NearObjects.size();
		
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
			const CONear& near = m_Parent->m_NearObjects[m_Index];
			CircleObject& obj = m_Parent->m_Objects[m_Index];
			if (near.RadiusNearArea == 0.0 || !IsNear(near.LastCenter, near.RadiusNearArea / 2, obj))
			{
				near.LastCenter 	= obj.Center;
				near.RadiusNearArea	= obj.Radius * 10.0;
				m_Parent->CalculateNear(&near, m_Index);	
			}
			nearSize = near.NearObjects.size();
		}
	}
		
	CrossNearSelector::CrossNearSelector()
	{		
	}
	
	void CrossNearSelector::Add(const CircleObjectPtr& a_Object)
	{
		m_Objects.push_back(a_Object);
		m_NearObjects.resize(m_Objects.size());
	}
	
	void CrossNearSelector::Delete(const Iterator& a_Iterator)
	{
		m_Objects.erase(m_Objects.begin() + a_Iterator.m_Index);
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

	Iterator CrossNearSelector::Begin()
	{
		return Iterator(this);
	}
	
		
	CrossNearSelector::CONear::CONear()
	{
		RadiusNearArea 	= 0.0;
	}
	
	bool CrossNearSelector::IsNear(Point a_Center, CoordinateType a_Radius, const CircleObjectPtr& a_Object)
	{
		const Point centerMinusObjCenter = a_Center - a_Object.Center;
		const CoordinateType radiusPlusObjRadius = a_Object.Radius + a_Radius;
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
		a_NearData->NearObjects.clear();
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