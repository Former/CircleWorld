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
		
		void 			Add(const CircleObjectPtr& a_Object);
		void 			Delete(const Iterator& a_Iterator);
		virtual void 	Delete(const CircleObjectPtr& a_Object);

		Iterator 		Begin();
		
	protected:		
		std::vector<CircleObjectPtr> m_Objects;
	};
		
	// Выбирает все пары объектов
	class CrossSelector : public Selector 
	{
		friend class Iterator;		
	public:
		class Iterator
		{
			friend class CrossSelector;
		public:
			Iterator(CrossSelector* a_Parent);

			CircleObjectPtr Get() const;
			bool IsEnd() const;
			void Next();
		
		private:
			size_t 	m_Index1;
			size_t 	m_Index2;
			CrossSelector* m_Parent;
		};
		
		CrossSelector();
		
		void 			Add(const CircleObjectPtr& a_Object);
		void 			Delete(const Iterator& a_Iterator);
		virtual void 	Delete(const CircleObjectPtr& a_Object);

		Iterator 		Begin();
		
	protected:		
		std::vector<CircleObjectPtr> m_Objects;
	};

	// Выбирает все пары объектов расположенных близко друг другу
	class CrossNearSelector : public Selector 
	{
		friend class Iterator;		
	public:
		class Iterator
		{
			friend class CrossNearSelector;
		public:
			Iterator(CrossNearSelector* a_Parent);

			CircleObjectPtr Get() const;
			bool IsEnd() const;
			void Next();
		
		private:
			void CheckNear();
		
			size_t 	m_Index;
			size_t 	m_NearIndex;
			CrossNearSelector* m_Parent;
		};
		
		CrossNearSelector();
		
		void 			Add(const CircleObjectPtr& a_Object);
		void 			Delete(const Iterator& a_Iterator);
		virtual void 	Delete(const CircleObjectPtr& a_Object);

		Iterator 		Begin();
		
	protected:
		class CONear
		{
		public:
			CONear();

			Point 							LastCenter;
			CoordinateType					RadiusNearArea;
			std::vector<CircleObjectPtr> 	NearObjects;
		};
	
		bool IsNear(Point a_Center, CoordinateType a_Radius, const CircleObjectPtr& a_Obj);
		void CalculateNear(CONear* a_NearData, size_t a_CurIndex);

		std::vector<CircleObjectPtr> 	m_Objects;
		std::vector<CONear> 			m_NearObjects;
	};
}
