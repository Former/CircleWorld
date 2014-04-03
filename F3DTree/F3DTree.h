#pragma once

#include <vector>
#include <tr1/memory>
#include "IntPoint.h"
#include <stdint.h>

#ifdef _DEBUG
#define ASSERT(x) do{ if(!(x)) { asm volatile ("int3;"); } }while(0)
#else
#define ASSERT(x) do{ }while(0)
#endif

#pragma pack (push, 1)

typedef int16_t CoordType;
typedef CPoint<CoordType> TreePoint;

size_t PoiterToIndex(const TreePoint& a_CurPoint);
const TreePoint& IndexToPoiter(const size_t& a_CurIndex);
TreePoint ConvertPoiterToNewIndex(const TreePoint& a_CurPoint, const size_t& a_CurIndex, const size_t& a_NewIndex);

template <typename ItemType>
class F3DTreeNode : public std::enable_shared_from_this<F3DTreeNode<ItemType>>
{
public:
	typedef std::shared_ptr<F3DTreeNode<ItemType>> F3DTreeNodePtr;
	typedef unsigned char LayerIndex;
	
	typedef CoordType CoordTreeType;
	typedef TreePoint Point;
	typedef BBox<CoordType> TreeBBox;
	
	struct ItemWithNode
	{
		ItemWithNode(const ItemType& a_Item, const F3DTreeNodePtr& a_Node)
		: m_Item(a_Item), m_Node(a_Node) 
		{	}
		
		ItemType m_Item;
		F3DTreeNodePtr m_Node;
	};
	
	static F3DTreeNodePtr CreateNode(const F3DTreeNodePtr& a_Parent, const ItemType& a_CurItem, const Point& a_CurPosition, const LayerIndex& a_CurrentLayerIndex)
	{
		return std::make_shared<F3DTreeNode<ItemType>>(a_Parent, a_CurItem, a_CurPosition, a_CurrentLayerIndex);
	}

	static F3DTreeNodePtr CreateTree(const ItemType& a_CurItem, const LayerIndex& a_CurrentLayerIndex)
	{
		return CreateNode(F3DTreeNodePtr(), a_CurItem, Point(0, 0, 0), a_CurrentLayerIndex);
	}

	F3DTreeNode(const F3DTreeNodePtr& a_Parent, const ItemType& a_CurItem, const Point& a_CurPosition, const LayerIndex& a_CurrentLayerIndex)
	{
		for (size_t i = 0; i < GetItemsCount(); ++i)
			m_Items[i] = a_CurItem;
			
		m_Parent = a_Parent;
		m_CurPosition = a_CurPosition;
		m_CurrentLayerIndex = a_CurrentLayerIndex;
		m_ChildNodes = 0;
	}

	const F3DTreeNodePtr* GetChildNodes() const
	{
		return m_ChildNodes;
	}

	size_t GetChildNodesCount() const
	{
		return (m_ChildNodes) ? 8 : 0;
	}

	ItemType* GetItems()
	{
		return m_Items;
	}

	size_t GetItemsCount() const
	{
		return 8;
	}
	
	const Point& GetCurPosition() const
	{
		return m_CurPosition;
	} 
	
	TreeBBox GetBBox() const
	{
		const size_t sdvig = GetCurrentLayerIndex() + 1;
		Point start(m_CurPosition.x << sdvig, m_CurPosition.y << sdvig, m_CurPosition.z << sdvig);
		Point diff(GetLength(), GetLength(), GetLength());
		
		return TreeBBox(start, start + diff);
	}

	LayerIndex GetCurrentLayerIndex() const
	{
		return m_CurrentLayerIndex;
	}
	
	const F3DTreeNodePtr& GetParent() const
	{
		return m_Parent;
	}

	ItemWithNode GetItemWithNode(const Point& a_TargetNodePosition, const LayerIndex& a_TargetLayerIndex = 0)
	{
		const size_t child_index = GetChildIndex(a_TargetNodePosition, a_TargetLayerIndex);
		
		if ((m_CurrentLayerIndex == a_TargetLayerIndex) || !GetChildNodesCount() || !m_ChildNodes[child_index])
			return ItemWithNode(m_Items[child_index], this->shared_from_this());

		return m_ChildNodes[child_index]->GetItemWithNode(a_TargetNodePosition, a_TargetLayerIndex);		
	}
	
	void SetItem(const ItemType& a_TargetItem, const Point& a_TargetNodePosition, const LayerIndex& a_TargetLayerIndex = 0)
	{
		const size_t child_index = GetChildIndex(a_TargetNodePosition, a_TargetLayerIndex);
		
		if ((m_CurrentLayerIndex != a_TargetLayerIndex) && CreateChild(child_index))
			return m_ChildNodes[child_index]->SetItem(a_TargetItem, a_TargetNodePosition, a_TargetLayerIndex);		

		m_Items[child_index] = a_TargetItem;
		if (m_Parent)
			m_Parent->OnChangeChildNode(m_Items, a_TargetNodePosition, a_TargetLayerIndex);
	}

	size_t GetLength() const
	{
		return (0x2 << static_cast<size_t>(m_CurrentLayerIndex));
	}
	
	F3DTreeNode<ItemType>& GetRoot()
	{
		if (!m_Parent)
			return this;
			
		return m_Parent->GetRoot();
	}

	bool Pack()
	{
		for (size_t i = 0 ; i < GetChildNodesCount(); ++i)
		{
			F3DTreeNodePtr& cur_node = m_ChildNodes[i];
			
			if (cur_node && cur_node->Pack())
				cur_node.reset();
		}
		
		if (NeedPackChildNodes())
			ClearChildNodes();

		if (!GetChildNodesCount())
			return CheckForDelete();
		
		return false;
	}
	
	bool CreateAllChild()
	{
		for (size_t i = 0; i < GetItemsCount(); ++i)
		{
			if (!CreateChild(i))
				return false;
		}
		
		return true;
	}
	
private:
	bool CheckForDelete() const
	{
		for (size_t i = 0; i < GetItemsCount(); ++i)
		{
			const ItemType& zero_type = m_Items[0];
			if (!(m_Items[i] == zero_type))
				return false;				
		}			
		
		return true;
	}
	
	bool NeedPackChildNodes() const
	{
		for (size_t i = 0 ; i < GetChildNodesCount(); ++i)
		{
			if (m_ChildNodes[i])
				return false;		
		}
		
		return true;
	}
	
	size_t GetChildIndex(const Point& a_TargetNodePosition, const LayerIndex& a_TargetLayerIndex = 0) const
	{
		Point conv_child_node_position = ConvertPoiterToNewIndex(a_TargetNodePosition, a_TargetLayerIndex, m_CurrentLayerIndex);
		
		Point child_node_point = conv_child_node_position - (m_CurPosition * static_cast<CoordType>(2));

		return PoiterToIndex(child_node_point);
	}
	
	void OnChangeChildNode(const ItemType* a_ChildNodeItems, const Point& a_TargetNodePosition, const LayerIndex& a_TargetLayerIndex)
	{
		const size_t child_index = GetChildIndex(a_TargetNodePosition, a_TargetLayerIndex);
		
		m_Items[child_index] = ItemType(a_ChildNodeItems, GetItemsCount());
		
		if (m_Parent)
			m_Parent->OnChangeChildNode(m_Items, a_TargetNodePosition, a_TargetLayerIndex);
	}
	
	bool CreateChild(const size_t& a_ChildIndex)
	{
		ASSERT(a_ChildIndex < GetItemsCount());
		
		if (GetChildNodesCount() && m_ChildNodes[a_ChildIndex])
			return true;
			
		if (m_CurrentLayerIndex == 0)
			return false;
		
		Point child_pointer = IndexToPoiter(a_ChildIndex) + (m_CurPosition * static_cast<CoordType>(2));
		
		if (!GetChildNodesCount())
			InitChildNodes();
		
		m_ChildNodes[a_ChildIndex] = CreateNode(this->shared_from_this(), m_Items[a_ChildIndex], child_pointer, m_CurrentLayerIndex - 1); 
		
		return true;
	}
	
	void InitChildNodes()
	{
		m_ChildNodes = new F3DTreeNodePtr[8];
		
		for (size_t i = 0; i < 8; ++i)
			m_ChildNodes[i] = F3DTreeNodePtr();
	}
	
	void ClearChildNodes()
	{
		delete [] m_ChildNodes;
		m_ChildNodes = 0;
	}

	F3DTreeNodePtr* m_ChildNodes;
	F3DTreeNodePtr m_Parent;
	ItemType m_Items[8];
	Point m_CurPosition;
	LayerIndex m_CurrentLayerIndex;
};

#pragma pack ( pop)
