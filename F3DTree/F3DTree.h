#pragma once

#include <vector>
#include <memory>
#include <IntPoint.h>

size_t PoiterToIndex(const IntPoint& a_CurPoint);
const IntPoint& IndexToPoiter(const size_t& a_CurIndex);
IntPoint ConvertPoiterToNewIndex(const IntPoint& a_CurPoint, const size& a_CurIndex, const size& a_NewIndex);

template <typename ItemType>
class F3DTreeNode : public std::enable_shared_from_this<F3DTreeNode<ItemType>>
{
public:
	typedef std::spared_ptr<F3DTreeNode<ItemType>> F3DTreeNodePtr;
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
	
	F3DTreeNode(const F3DTreeNodePtr& a_Parent, const ItemType& a_CurItem, const IntPoint& a_CurPosition, const size_t& a_ChildNodesCount)
	: m_ChildNodes, m_Items(8, a_CurItem)
	{
		m_Parent = a_Parent;
		m_CurPosition = a_CurPosition;
		m_ChildNodesCount = a_ChildNodesCount;
	}
	
	const NodeVector& GetChildNodes() const
	{
		return m_ChildNodes;
	}

	const ItemVector& GetItems() const
	{
		return m_Items;
	}

	const size_t& GetChildNodesCount() const
	{
		return m_ChildNodesCount;
	}
	
	const F3DTreeNodePtr& GetParent() const
	{
		return m_Parent;
	}

	const ItemWithNode& GetItemWithNode(const IntPoint& a_TargetNodePosition, const size_t& a_TargetChildNodeCount = 0) const
	{
		const size_t child_index = GetChildIndex(a_TargetNodePosition, a_TargetChildNodeCount);
		
		if (m_ChildNodes.empty() || !m_ChildNodes[child_index])
			return ItemWithNode(m_Items[child_index], shared_from_this());

		return m_ChildNodes[child_index]->GetItemWithNode(a_TargetNodePosition, a_TargetChildNodeCount);		
	}
	
	void SetItem(const ItemType& a_TargetItem, const IntPoint& a_TargetNodePosition, const size_t& a_TargetChildNodeCount = 0) const
	{
		const size_t child_index = GetChildIndex(a_TargetNodePosition, a_TargetChildNodeCount);
		
		if (!CreateChild(child_index))
		{
			m_Items[child_index] = a_TargetItem;
			if (m_Parent)
				m_Parent->OnChangeChildNode(m_Items, a_TargetNodePosition, a_TargetChildNodeCount);
		}

		m_ChildNodes[child_index]->SetItem(a_TargetItem, a_TargetNodePosition, a_TargetChildNodeCount);		
	}

	size_t GetLeght() const
	{
		return (u0x1 << m_ChildNodesCount);
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
			const F3DTreeNodePtr& cur_node = m_ChildNodes[i];
			
			if (cur_node && cur_node->Pack())
				cur_node.reset();
		}
		
		if (NeedPackChildNodes())
			m_ChildNodes.clear();

		if (m_ChildNodes.empty())
			return CheckForDelete();
	}
	
private:
	bool CheckForDelete() const
	{
		for (size_t i = 0 ; i < m_Items.size(); ++i)
		{
			const ItemType& zero_type = m_Items[0];
			if (m_Items[i] != zero_type)
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
	
	bool CreateChild(const size_t& a_ChildIndex)
	{
		if (!m_ChildNodes.empty() && m_ChildNodes[a_ChildIndex])
			return true;
			
		if (m_ChildNodesCount == 0)
			return false;
		
		IntPoint child_pointer = IndexToPoiter(a_ChildIndex) + 2 * m_CurPosition;
		
		if (m_ChildNodes.empty())
			m_ChildNodes.resize(8);
		
		m_ChildNodes[a_ChildIndex] = boost::make_shared<F3DTreeNode<ItemType>>(shared_from_this(), m_Items[a_ChildIndex], child_pointer, m_ChildNodesCount - 1); 
		
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

	F3DTreeNodePtr m_Parent;
	NodeVector m_ChildNodes;
	ItemVector m_Items;
	IntPoint m_CurPosition;
	size_t m_ChildNodesCount;
};
