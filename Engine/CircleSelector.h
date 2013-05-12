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
	class PairNearSelector : public PairSelector 
	{
		friend class Iterator;		
	public:
		class Iterator : public PairSelector::Iterator
		{
			friend class PairNearSelector;
		public:
			Iterator(PairNearSelector* a_Parent)
			{
				m_ObjectIndex = 0;
			}
			
			virtual ~Iterator();

			virtual CircleObjectPtr GetFirst() const;
			virtual CircleObjectPtr GetSecond() const;
			virtual bool IsEnd() const;
			virtual void Next();
		
		private:
			void FindNextFullNear();
		
			size_t 	m_ObjectIndex;
			size_t 	m_NearIndex;
			PairNearSelector* m_Parent;
		};
		
		PairNearSelector(const CoordinateType& a_ZeroAreaSize)
		{
			m_ZeroAreaSize = a_ZeroAreaSize;
		}
		
		virtual ~PairNearSelector()
		{
			
		}
		
		void 	Add(const CircleObjectPtr& a_Object)
		{
			ObjectPtr new_obj = engine_make_shared<Object>();
			new_obj->m_CircleObject = a_Object;
			m_Objects.push_back(new_obj);
		}
		
		virtual void 	Delete(const CircleObjectPtr& a_Object)
		{
			//...
		}
		
		static std::vector<AreaPtr> GetNearAreas(const CircleObjectPtr& a_Object, const OrderAreaType& a_Order)
		{
			CoordinateType l_1DivAreaSize = 1.0 / (m_ZeroAreaSize * pow(2.0, a_Order));
			Point boundary_vectors[] = 
			{
				Point(0.0, 0.0, 0.0),
				Point(a_Object->Radius, a_Object->Radius, a_Object->Radius),
				Point(a_Object->Radius, a_Object->Radius, -a_Object->Radius),
				Point(a_Object->Radius, -a_Object->Radius, a_Object->Radius),
				Point(a_Object->Radius, -a_Object->Radius, -a_Object->Radius),
				Point(-a_Object->Radius, a_Object->Radius, a_Object->Radius),
				Point(-a_Object->Radius, a_Object->Radius, -a_Object->Radius),
				Point(-a_Object->Radius, -a_Object->Radius, a_Object->Radius),
				Point(-a_Object->Radius, -a_Object->Radius, -a_Object->Radius),
			};
			
			std::vector<AreaPtr> result;
			
			for (size_t i = 0; i < sizeof(boundary_vectors) / sizeof(boundary_vectors[0]); ++i)
			{
				Point cur_point = a_Object->Center + boundary_vectors[i];
				CoordinateAreaType x = (CoordinateAreaType)(cur_point.x * l_1DivAreaSize);
				CoordinateAreaType y = (CoordinateAreaType)(cur_point.y * l_1DivAreaSize);
				CoordinateAreaType z = (CoordinateAreaType)(cur_point.z * l_1DivAreaSize);
				
				AreaPtr cur_area = m_Areas.Get(x, y, z, order);
				
				if (std::find(result.begin(), result.end(), cur_area) == result.end())
					result.push_back(cur_area);
			}
			
			return result;
		}

		virtual PairSelector::IteratorPtr Begin()
		{
			for (size_t i = 0; i < m_Objects.size(); ++i)
			{
				const ObjectPtr& cur_obj = m_Objects[i];
				OrderAreaType order = GetOrder(cur_obj->m_CircleObject->Radius, m_ZeroAreaSize, 1.0);
				
				std::vector<AreaPtr> new_areas = GetNearAreas(cur_obj->m_CircleObject, order);
				std::vector<AreaPtr>& old_areas = cur_obj->NearAreas;
				
				for (size_t j = 0; j < new_areas.size(); ++j)
				{
					AreaPtr& cur_area = new_areas[j];
					if (std::find(old_areas.begin(), old_areas.end(), cur_area) == old_areas.end())
						cur_area->AddObject(cur_obj);					
				}

				for (size_t j = 0; j < old_areas.size(); ++j)
				{
					AreaPtr& cur_area = old_areas[j];
					if (std::find(new_areas.begin(), new_areas.end(), cur_area) == new_areas.end())
						cur_area->DeleteObject(cur_obj);					
				}
				
				old_areas = new_areas;
			}
		}
		
	protected:
		class Object;
		typedef engine_shared_ptr<Object> ObjectPtr;

		class Area;
		typedef engine_shared_ptr<Area> AreaPtr;

		class Object
		{
		public:
			CircleObjectPtr m_CircleObject;
			std::vector<AreaPtr> NearAreas;
		};

		typedef int CoordinateAreaType;
		typedef int OrderAreaType;

		static OrderAreaType GetOrder(const CoordinateType& a_Radius, const CoordinateType& a_ZeroAreaSize, const double& a_Multiple)
		{
			OrderAreaType znak = 1.0;
			
			CoordinateType div = a_Radius * a_Multiple / a_ZeroAreaSize;
			if (div == 0.0)
				return 0;
				
			if (div < 1.0)
			{
				div = 1.0 / div;
				znak = -1.0;
			}
			
			return znak * log(div) / log(2.0);
		}
		
		class Area
		{
		public:
			Area()
			{
				x = 0;
				y = 0;
				z = 0;
				m_Order = 1;
			}
			Area(const CoordinateAreaType& a_X, const CoordinateAreaType& a_Y, const CoordinateAreaType& a_Z, const OrderAreaType& a_Order)
			{
				x = a_X;
				y = a_Y;
				z = a_Z;
				m_Order = a_Order;
			}
			
			CoordinateAreaType x;
			CoordinateAreaType y;
			CoordinateAreaType z;
			OrderAreaType m_Order;			
		
			void AddObject(const ObjectPtr& a_Object)
			{
				for (size_t i = 0; i < m_CircleObject.size(); ++i)
				{
					if (a_Object == m_CircleObject[i])
						return;
				}
				
				m_CircleObject.push_back(a_Object);
			}

			void DeleteObject(const ObjectPtr& a_Object)
			{
				for (size_t i = 0; i < m_CircleObject.size(); ++i)
				{
					if (a_Object == m_CircleObject[i])
					{
						m_CircleObject.erase(i);
						break;		
					}						
				}
			}
			
			const std::vector<ObjectPtr>& GetObjects() const
			{
				return m_CircleObject;
			}
		
		private:
			std::vector<ObjectPtr> m_CircleObject;
		};
		
		class AreaContainer
		{
			const CoordinateAreaType s_MASK_COORDINATE = 0x3F;
		public:
			AreaPtr Get(const CoordinateAreaType& a_X, const CoordinateAreaType& a_Y, const CoordinateAreaType& a_Z, const OrderAreaType& a_Order)
			{
				Container::iterator it_order = m_Container.find(a_Order);
				
				VectorZ* cur_vector_z = 0;
				if (it_order != m_Container.end())
					cur_vector_z = &(it_order->second);
				else
				{
					cur_vector_z = &(m_Container[a_Order]);
					cur_vector_z->resize(s_MASK_COORDINATE + 1);
					
					for (size_t j = 0; j < cur_vector_z->size(); ++j)
					{
						VectorX& cur_vector_y = (*cur_vector_z)[j];
						cur_vector_y.resize(s_MASK_COORDINATE + 1);
						for (size_t i = 0; i < cur_vector_y.size(); ++i)
						{
							VectorX& cur_vector_x = cur_vector_y[i];
							cur_vector_x.resize(s_MASK_COORDINATE + 1);
						}
					}
				}
				
				Areas& cur_areas = (*cur_vector_z)[a_Z & s_MASK_COORDINATE][a_Y & s_MASK_COORDINATE][a_X & s_MASK_COORDINATE];
				for (size_t i = 0; i < cur_areas.size(); ++i)
				{
					AreaPtr& cur_area = cur_areas[i];
					if (cur_area.x == a_X && cur_area.y == a_Y && cur_area.z == a_Z && cur_area.m_Order == a_Order)
						return cur_area;
				}
				
				AreaPtr cur_area = engine_make_shared<Area>(a_X, a_Y, a_Z, a_Order);
				cur_areas.push_back(cur_area);
				
				return cur_area;
			}
			
		private:
			typedef std::vector<AreaPtr> Areas;
			typedef std::vector<Areas> VectorX;
			typedef std::vector<VectorX> VectorY;
			typedef std::vector<VectorY> VectorZ;
			typedef std::map<OrderAreaType, VectorY> Container;
			
			Container m_Container;
		};
	
		std::vector<ObjectPtr> 	m_Objects;
		AreaContainer           m_Areas;
		CoordinateType          m_ZeroAreaSize;
	};
	typedef engine_shared_ptr<PairNearSelector> PairNearSelectorPtr;
	
	///////////////////////////////////////////////////////////////////////
	
	// Выбирает все пары объектов, которые были установлены пользователем
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
