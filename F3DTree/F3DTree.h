#pragma once

#include <vector>
#include <tr1/memory>
#include "IntPoint.h"

#ifdef _DEBUG
#define ASSERT(x) do{ if(!(x)) { asm volatile ("int3;"); } }while(0)
#else
#define ASSERT(x) do{ }while(0)
#endif

size_t PoiterToIndex(const IntPoint& a_CurPoint);
const IntPoint& IndexToPoiter(const size_t& a_CurIndex);
IntPoint ConvertPoiterToNewIndex(const IntPoint& a_CurPoint, const size_t& a_CurIndex, const size_t& a_NewIndex);

template <typename ItemType>
class F3DTreeNode : public std::enable_shared_from_this<F3DTreeNode<ItemType>>
{
public:
	typedef std::shared_ptr<F3DTreeNode<ItemType>> F3DTreeNodePtr;
	typedef std::vector<F3DTreeNodePtr> NodeVector;
	typedef std::vector<ItemType> ItemVector;
	
	struct ItemWithNode
	{
		ItemWithNode(const ItemType& a_Item, const F3DTreeNodePtr& a_Node)
		: m_Item(a_Item), m_Node(a_Node) 
		{	}
		
		ItemType m_Item;
		F3DTreeNodePtr m_Node;
	};
	
	static F3DTreeNodePtr CreateNode(const F3DTreeNodePtr& a_Parent, const ItemType& a_CurItem, const IntPoint& a_CurPosition, const size_t& a_ChildNodesCount)
	{
		return std::make_shared<F3DTreeNode<ItemType>>(a_Parent, a_CurItem, a_CurPosition, a_ChildNodesCount);
	}

	static F3DTreeNodePtr CreateTree(const ItemType& a_CurItem, const size_t& a_ChildNodesCount)
	{
		return CreateNode(F3DTreeNodePtr(), a_CurItem, IntPoint(0, 0, 0), a_ChildNodesCount);
	}

	F3DTreeNode(const F3DTreeNodePtr& a_Parent, const ItemType& a_CurItem, const IntPoint& a_CurPosition, const size_t& a_ChildNodesCount)
	: m_Items(8, a_CurItem)
	{
		m_Parent = a_Parent;
		m_CurPosition = a_CurPosition;
		m_ChildNodesCount = a_ChildNodesCount;
	}

	const NodeVector& GetChildNodes() const
	{
		return m_ChildNodes;
	}

	ItemVector& GetItems()
	{
		return m_Items;
	}
	
	const IntPoint& GetCurPosition() const
	{
		return m_CurPosition;
	} 

	const size_t& GetChildNodesCount() const
	{
		return m_ChildNodesCount;
	}
	
	const F3DTreeNodePtr& GetParent() const
	{
		return m_Parent;
	}

	ItemWithNode GetItemWithNode(const IntPoint& a_TargetNodePosition, const size_t& a_TargetChildNodeCount = 0)
	{
		const size_t child_index = GetChildIndex(a_TargetNodePosition, a_TargetChildNodeCount);
		
		if ((m_ChildNodesCount == a_TargetChildNodeCount) || m_ChildNodes.empty() || !m_ChildNodes[child_index])
			return ItemWithNode(m_Items[child_index], this->shared_from_this());

		return m_ChildNodes[child_index]->GetItemWithNode(a_TargetNodePosition, a_TargetChildNodeCount);		
	}
	
	void SetItem(const ItemType& a_TargetItem, const IntPoint& a_TargetNodePosition, const size_t& a_TargetChildNodeCount = 0)
	{
		const size_t child_index = GetChildIndex(a_TargetNodePosition, a_TargetChildNodeCount);
		
		if ((m_ChildNodesCount != a_TargetChildNodeCount) && CreateChild(child_index))
			return m_ChildNodes[child_index]->SetItem(a_TargetItem, a_TargetNodePosition, a_TargetChildNodeCount);		

		m_Items[child_index] = a_TargetItem;
		if (m_Parent)
			m_Parent->OnChangeChildNode(m_Items, a_TargetNodePosition, a_TargetChildNodeCount);
	}

	size_t GetLength() const
	{
		return (0x1 << m_ChildNodesCount);
	}
	
	F3DTreeNode& GetRoot()
	{
		if (!m_Parent)
			return this;
			
		return m_Parent->GetRoot();
	}

	bool Pack()
	{
		for (size_t i = 0 ; i < m_ChildNodes.size(); ++i)
		{
			F3DTreeNodePtr& cur_node = m_ChildNodes[i];
			
			if (cur_node && cur_node->Pack())
				cur_node.reset();
		}
		
		if (NeedPackChildNodes())
			m_ChildNodes.clear();

		if (m_ChildNodes.empty())
			return CheckForDelete();
		
		return false;
	}
	
	bool CreateAllChild()
	{
		for (size_t i = 0; i < m_Items.size(); ++i)
		{
			if (!CreateChild(i))
				return false;
		}
		
		return true;
	}
	
private:
	bool CheckForDelete() const
	{
		for (size_t i = 0 ; i < m_Items.size(); ++i)
		{
			const ItemType& zero_type = m_Items[0];
			if (!(m_Items[i] == zero_type))
				return false;				
		}			
		
		return true;
	}
	
	bool NeedPackChildNodes() const
	{
		for (size_t i = 0 ; i < m_ChildNodes.size(); ++i)
		{
			if (m_ChildNodes[i])
				return false;		
		}
		
		return true;
	}
	
	size_t GetChildIndex(const IntPoint& a_TargetNodePosition, const size_t& a_TargetChildNodeCount = 0) const
	{
		IntPoint conv_child_node_position = ConvertPoiterToNewIndex(a_TargetNodePosition, a_TargetChildNodeCount, m_ChildNodesCount);
		
		IntPoint child_node_point = conv_child_node_position - (m_CurPosition * 2);

		return PoiterToIndex(child_node_point);
	}
	
	void OnChangeChildNode(const ItemVector& a_ChildNodeItems, const IntPoint& a_TargetNodePosition, const size_t& a_TargetChildNodeCount)
	{
		const size_t child_index = GetChildIndex(a_TargetNodePosition, a_TargetChildNodeCount);
		
		m_Items[child_index] = ItemType(a_ChildNodeItems);
		
		if (m_Parent)
			m_Parent->OnChangeChildNode(m_Items, a_TargetNodePosition, a_TargetChildNodeCount);
	}
	
	bool CreateChild(const size_t& a_ChildIndex)
	{
		ASSERT(a_ChildIndex < 8);
		
		if (!m_ChildNodes.empty() && m_ChildNodes[a_ChildIndex])
			return true;
			
		if (m_ChildNodesCount == 0)
			return false;
		
		IntPoint child_pointer = IndexToPoiter(a_ChildIndex) + (m_CurPosition * 2);
		
		if (m_ChildNodes.empty())
			m_ChildNodes.resize(8);
		
		m_ChildNodes[a_ChildIndex] = CreateNode(this->shared_from_this(), m_Items[a_ChildIndex], child_pointer, m_ChildNodesCount - 1); 
		
		return true;
	}

	F3DTreeNodePtr m_Parent;
	NodeVector m_ChildNodes;
	ItemVector m_Items;
	IntPoint m_CurPosition;
	size_t m_ChildNodesCount;
};
