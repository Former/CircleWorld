#pragma once

#include "CircleObject.h"

namespace CircleEngine
{
	class Selector
	{
	public:
		virtual ~Selector();
		
		virtual void Delete(const CircleObjectPtr& a_Object) = 0;
	};
	
	typedef engine_shared_ptr<Selector> SelectorPtr;

	// Последовательно выбирает объекты
	class SequenceSelector : public Selector
	{
		friend class Iterator;		
	public:
		class Iterator
		{
			friend class SequenceSelector;
		public:
			Iterator(SequenceSelector* a_Parent);

			CircleObjectPtr Get() const;
			bool IsEnd() const;
			void Next();
		
		private:
			size_t m_Index;
			SequenceSelector* m_Parent;
		};
		
		SequenceSelector();
		virtual ~SequenceSelector();
		
		void 			Add(const CircleObjectPtr& a_Object);
		virtual void 	Delete(const CircleObjectPtr& a_Object);

		Iterator 		Begin();
		
	protected:		
		std::vector<CircleObjectPtr> m_Objects;
	};
	typedef engine_shared_ptr<SequenceSelector> SequenceSelectorPtr;
	
	// Выбирает все пары объектов
	class PairSelector : public Selector 
	{
	public:
		class Iterator
		{
		public:
			virtual ~Iterator();

			virtual CircleObjectPtr GetFirst() const = 0;
			virtual CircleObjectPtr GetSecond() const = 0;
			virtual bool IsEnd() const = 0;
			virtual void Next() = 0;
		};
		typedef engine_shared_ptr<Iterator> IteratorPtr;
		
		virtual ~PairSelector();
		
		virtual void 	Delete(const CircleObjectPtr& a_Object) = 0;

		virtual IteratorPtr Begin() = 0;
	};
	typedef engine_shared_ptr<PairSelector> PairSelectorPtr;

	//////////////////////////////////////////////////////////////

	class SomeToOtherSelector : public PairSelector 
	{
		friend class Iterator;		
	public:
		class Iterator : public PairSelector::Iterator
		{
			friend class SomeToOtherSelector;
		public:
			Iterator(SomeToOtherSelector* a_Parent);
			virtual ~Iterator();

			virtual CircleObjectPtr GetFirst() const;
			virtual CircleObjectPtr GetSecond() const;
			virtual bool IsEnd() const;
			virtual void Next();
		
		private:
			size_t 	m_SomeIndex;
			size_t 	m_OtherIndex;
			SomeToOtherSelector* m_Parent;
		};
		
		SomeToOtherSelector();
		virtual ~SomeToOtherSelector();
		
		void 	AddSome(const CircleObjectPtr& a_Object);
		void 	AddOther(const CircleObjectPtr& a_Object);
		virtual void 	Delete(const CircleObjectPtr& a_Object);

		virtual PairSelector::IteratorPtr Begin();
		
	protected:		
		std::vector<CircleObjectPtr> m_SomeObjects;
		std::vector<CircleObjectPtr> m_OtherObjects;
	};
	typedef engine_shared_ptr<SomeToOtherSelector> SomeToOtherSelectorPtr;

	//////////////////////////////////////////////////////////////
	
	class CrossSelector : public PairSelector 
	{
		friend class Iterator;		
	public:
		class Iterator : public PairSelector::Iterator
		{
			friend class CrossSelector;
		public:
			Iterator(CrossSelector* a_Parent);
			virtual ~Iterator();

			virtual CircleObjectPtr GetFirst() const;
			virtual CircleObjectPtr GetSecond() const;
			virtual bool IsEnd() const;
			virtual void Next();
		
		private:
			size_t 	m_Index1;
			size_t 	m_Index2;
			CrossSelector* m_Parent;
		};
		
		CrossSelector();
		virtual ~CrossSelector();
		
		void 	Add(const CircleObjectPtr& a_Object);
		virtual void 	Delete(const CircleObjectPtr& a_Object);

		virtual PairSelector::IteratorPtr Begin();
		
	protected:		
		std::vector<CircleObjectPtr> m_Objects;
	};
	typedef engine_shared_ptr<CrossSelector> CrossSelectorPtr;

	// Выбирает все пары объектов, расположенных близко друг другу
	class CrossNearSelector : public PairSelector 
	{
		friend class Iterator;		
	public:
		class Iterator : public PairSelector::Iterator
		{
			friend class CrossNearSelector;
		public:
			Iterator(CrossNearSelector* a_Parent);
			virtual ~Iterator();

			virtual CircleObjectPtr GetFirst() const;
			virtual CircleObjectPtr GetSecond() const;
			virtual bool IsEnd() const;
			virtual void Next();
		
		private:
			void FindNextFullNear();
		
			size_t 	m_Index;
			size_t 	m_NearIndex;
			CrossNearSelector* m_Parent;
		};
		
		CrossNearSelector(size_t a_BestNeighborsCount);
		virtual ~CrossNearSelector();
		
		void 	Add(const CircleObjectPtr& a_Object);
		virtual void 	Delete(const CircleObjectPtr& a_Object);

		virtual PairSelector::IteratorPtr Begin();
		
	protected:
		class CONear;
		typedef engine_shared_ptr<CONear> CONearPtr;

		class CONear
		{
		public:
			CONear();

			Point 							LastCenter;
			CoordinateType					RadiusNearArea;
			CoordinateType					RadiusMoveArea;
			std::vector<CircleObjectPtr> 	NearObjects;
			std::vector<CONearPtr> 			NearArray;
		};
	
		bool IsNear(const Point& a_Center, CoordinateType a_Radius, const Point& a_OtherCenter);
		void CalculateNear(const CONearPtr& a_NearData, size_t a_CurIndex);
		void CalculateNearFromAllObject(const CONearPtr& a_NearData, size_t a_CurIndex);
		void FillNearData(const CONearPtr& a_NearData, const CircleObjectPtr& a_Obj, const CONearPtr& a_ObjNear);

		size_t m_BestNeighborsCount;
		std::vector<CircleObjectPtr> 	m_Objects;
		std::vector<CONearPtr> 			m_NearObjects;
	};
	typedef engine_shared_ptr<CrossNearSelector> CrossNearSelectorPtr;
	
	///////////////////////////////////////////////////////////////////////
	
	// Выбирает все пары объектов, расположенных близко друг другу
	template <typename UserData>
	class PairUserSelector : public Selector 
	{
		friend class Iterator;		
	public:
		class Iterator
		{
			friend class PairUserSelector;
		public:
			Iterator(PairUserSelector* a_Parent)
			{
				m_Parent = a_Parent;
				m_Index = 0;
			}

			CircleObjectPtr GetFirst() const
			{
				return m_Parent->m_Objects[m_Index].first;
			}
			
			CircleObjectPtr GetSecond() const
			{
				return m_Parent->m_Objects[m_Index].second;
			}
			
			UserData 		GetUserData() const
			{
				return m_Parent->m_Objects[m_Index].data;
			}
			
			bool IsEnd() const
			{
				if (m_Index >= m_Parent->m_Objects.size())
					return true;
				return false;
			}
			
			void Next()
			{
				if (IsEnd())
					return;
				m_Index++;
			}
		
		private:
			size_t 	m_Index;
			PairUserSelector* m_Parent;
		};
		typedef engine_shared_ptr<Iterator> IteratorPtr;
		
		PairUserSelector()
		{			
		}
		
		virtual ~PairUserSelector()
		{			
		}
		
		void 	Add(const CircleObjectPtr& a_Object1, const CircleObjectPtr& a_Object2, UserData& a_UserData)
		{
			Item item;
			item.first = a_Object1;
			item.second = a_Object2;
			item.data = a_UserData;
			m_Objects.push_back(item);
		}
		
		virtual void 	Delete(const CircleObjectPtr& a_Object)
		{
			for(size_t i = 0; i < m_Objects.size(); ++i)
			{
				Item it = m_Objects[i];
				if ((it.first.get() == a_Object.get()) || (it.second.get() == a_Object.get()))
					m_Objects.erase(m_Objects.begin() + i);
			}
		}

		virtual IteratorPtr Begin()
		{
			return IteratorPtr(new Iterator(this));
		}
		
	protected:
		struct Item
		{
			CircleObjectPtr first;
			CircleObjectPtr second;
			UserData		data;
		};
	
		std::vector<Item> 	m_Objects;
	};
}
