#pragma once

#include "CircleObject.h"

namespace CircleEngine
{
	typedef std::pair<CircleObjectPtr, CircleObjectPtr> CircleObjectPair;
	
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

			virtual CircleObjectPair Get() const = 0;
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

			virtual CircleObjectPair Get() const;
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

			virtual CircleObjectPair Get() const;
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

			virtual CircleObjectPair Get() const;
			virtual bool IsEnd() const;
			virtual void Next();
		
		private:
			void FindNextFullNear();
		
			size_t 	m_Index;
			size_t 	m_NearIndex;
			CrossNearSelector* m_Parent;
		};
		
		CrossNearSelector();
		virtual ~CrossNearSelector();
		
		void 	Add(const CircleObjectPtr& a_Object);
		virtual void 	Delete(const CircleObjectPtr& a_Object);

		virtual PairSelector::IteratorPtr Begin();
		
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
	typedef engine_shared_ptr<CrossNearSelector> CrossNearSelectorPtr;
}
