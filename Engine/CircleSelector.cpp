#include "CircleSelector.h"

#include <set>

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
		const CONearPtr& near = m_Parent->m_NearObjects[m_Index];
		return near->NearObjects[m_NearIndex];
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
		const size_t nearSize = m_Parent->m_NearObjects[m_Index]->NearObjects.size();
		
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
			CONearPtr& near = m_Parent->m_NearObjects[m_Index];
			if (!near)
				near = CONearPtr(new CONear);
				
			CircleObjectPtr& obj = m_Parent->m_Objects[m_Index];
			if (near->RadiusMoveArea < obj->Radius || !m_Parent->IsNear(near->LastCenter, near->RadiusMoveArea, obj->Center))
			{
				near->LastCenter 	= obj->Center;
				if (near->RadiusNearArea < obj->Radius)
					near->RadiusNearArea	= obj->Radius * 10.0;
				else
				{
					if (near->NearObjects.size() > m_Parent->m_BestNeighborsCount)
						near->RadiusNearArea = near->RadiusNearArea * 0.70;
					else
						near->RadiusNearArea = near->RadiusNearArea * 1.3;
					
					if (near->RadiusNearArea < obj->Radius * 3.0)
						near->RadiusNearArea = obj->Radius * 3.0;						
				}		
				near->RadiusMoveArea = near->RadiusNearArea * 0.5;
				m_Parent->CalculateNear(near, m_Index);	
			}
			nearSize = near->NearObjects.size();
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
				m_NearObjects.erase(m_NearObjects.begin() + i);
				break;
			}
		}
		
		// toDo delete from nearArray
	}

	PairSelector::IteratorPtr CrossNearSelector::Begin()
	{
		return IteratorPtr(new Iterator(this));
	}
	
		
	CrossNearSelector::CONear::CONear()
	{
		RadiusNearArea 	= 0.0;
		RadiusMoveArea	= 0.0;
	}
	
	bool CrossNearSelector::IsNear(const Point& a_Center, CoordinateType a_Radius, const Point& a_OtherCenter)
	{
		const Point centerMinusObjCenter = a_Center - a_OtherCenter;
		if (centerMinusObjCenter.x > a_Radius || centerMinusObjCenter.x < -a_Radius )
			return false;
		if (centerMinusObjCenter.y > a_Radius || centerMinusObjCenter.y < -a_Radius )
			return false;
		if (centerMinusObjCenter.z > a_Radius || centerMinusObjCenter.z < -a_Radius )
			return false;
		return true;
	}
	
	void CrossNearSelector::CalculateNear(const CONearPtr& a_NearData, size_t a_CurIndex)
	{
		static int s_AllCount = 0;
		static int s_NearCount = 0;
		//CalculateNearFromAllObject(a_NearData, a_CurIndex);
		//return;

		const CircleObjectPtr& curObj 	= m_Objects[a_CurIndex];
		const CONearPtr& curNear 				= a_NearData;
		
		// Попробуем вычислить с  помощью ближайших объектов
		Point peaks[8]; // Вершины новой границы  видимости
		peaks[0] = curObj->Center + Point(curNear->RadiusNearArea, curNear->RadiusNearArea, curNear->RadiusNearArea);
		peaks[1] = curObj->Center + Point(curNear->RadiusNearArea, curNear->RadiusNearArea, -curNear->RadiusNearArea);
		peaks[2] = curObj->Center + Point(curNear->RadiusNearArea, -curNear->RadiusNearArea, curNear->RadiusNearArea);
		peaks[3] = curObj->Center + Point(curNear->RadiusNearArea, -curNear->RadiusNearArea, -curNear->RadiusNearArea);
		peaks[4] = curObj->Center + Point(-curNear->RadiusNearArea, curNear->RadiusNearArea, curNear->RadiusNearArea);
		peaks[5] = curObj->Center + Point(-curNear->RadiusNearArea, curNear->RadiusNearArea, -curNear->RadiusNearArea);
		peaks[6] = curObj->Center + Point(-curNear->RadiusNearArea, -curNear->RadiusNearArea, curNear->RadiusNearArea);
		peaks[7] = curObj->Center + Point(-curNear->RadiusNearArea, -curNear->RadiusNearArea, -curNear->RadiusNearArea);
		
		bool usePeaks[8] = {0}; // Покрыты ли вершины
		size_t usePeaksCount = 0;
		size_t nearIndex[8] = {0}; // Индексы соседьних объектов которые перекрыли вершины
		size_t nearIndexCount = 0;
		
		for (size_t i = 0; i < curNear->NearObjects.size(); i++)
		{
			CalculateNearFromAllObject(a_NearData, a_CurIndex);
			return;
			
			const CircleObjectPtr& obj 	= curNear->NearObjects[i];
			// должен обязательно перекрывать центр
			if (!IsNear(curObj->Center, curNear->RadiusNearArea, obj->Center))
				continue;
			
			const CONearPtr& near 		= curNear->NearArray[i];
			if (!near)
				continue;

			// Перекрывает ли вершины?
			bool isNearUsePeaks = false;
			for (size_t j = 0; j < 8; j++)
			{
				if (usePeaks[j])
					continue;
				
				if (IsNear(obj->Center, near->RadiusNearArea, peaks[j]))
				{
					usePeaks[j] = true;
					usePeaksCount++;
					isNearUsePeaks = true;
				}
			}
			
			if (isNearUsePeaks)
			{
				nearIndex[nearIndexCount] = i;
				nearIndexCount++;
			}
			
			if (usePeaksCount == 8)
				break;
		}
		
		if (usePeaksCount != 8)
		{
			s_AllCount++;
			// С помощью ближайших не удалось. Берем все объекты
			CalculateNearFromAllObject(a_NearData, a_CurIndex);
			return;
		}
		s_NearCount++;
		
		std::vector<CONearPtr> curNearArray = curNear->NearArray;
		a_NearData->NearObjects.resize(0);
		a_NearData->NearArray.resize(0);
		
		std::set<size_t> allIds;
		for (size_t i = 0; i < nearIndexCount; i++)
		{
			const CONearPtr& near 				= curNearArray[nearIndex[i]];
			for (size_t j = 0; j < near->NearObjects.size(); j++)
			{
				const CircleObjectPtr& obj 	= near->NearObjects[j];
				const CONearPtr& near1 		= near->NearArray[j];

				if (obj->GetID() == curObj->GetID())
					continue;
				
				if (allIds.find(obj->GetID()) != allIds.end())
					continue;
				
				allIds.insert(obj->GetID());
				FillNearData(a_NearData, obj, near1);
			}
		}		
	}

	void CrossNearSelector::FillNearData(const CONearPtr& a_NearData, const CircleObjectPtr& a_Obj, const CONearPtr& a_ObjNear)
	{
		CoordinateType radius = (a_ObjNear) ? a_ObjNear->RadiusMoveArea : a_Obj->Radius;
		if (IsNear(a_NearData->LastCenter, a_NearData->RadiusNearArea + radius, a_Obj->Center))
		{
			a_NearData->NearObjects.push_back(a_Obj);
			a_NearData->NearArray.push_back(a_ObjNear);
		}
	}
	
	void CrossNearSelector::CalculateNearFromAllObject(const CONearPtr& a_NearData, size_t a_CurIndex)
	{
		a_NearData->NearObjects.resize(0);
		a_NearData->NearArray.resize(0);
		const CircleObjectPtr& curObj 	= m_Objects[a_CurIndex];
		for (size_t i = 0; i < m_Objects.size(); i++)
		{
			if (i == a_CurIndex)
				continue;
			
			const CircleObjectPtr& obj 	= m_Objects[i];
			const CONearPtr& near 		= m_NearObjects[i];

			FillNearData(a_NearData, obj, near);
		}
	}
}