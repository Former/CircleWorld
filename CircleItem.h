#pragma once 

#include <vector>
#include "F3DTree/F3DTree.h"

#pragma pack (push, 1)

struct CircleItem
{
	enum Type : unsigned char
	{
		tpNone = 0,
		tpSolid,
		tpWater,
		tpCount,
	};

	CircleItem()
	{
		m_Type = tpNone;
	}

	CircleItem(const CircleItem::Type& a_Type)
	{
		m_Type = a_Type;
	}

	CircleItem(const CircleItem* a_Items, const size_t& a_Count)
	{
		if (a_Count)
			m_Type = a_Items[0].m_Type;
	}
	
	bool operator == (const CircleItem& a_Item) const
	{
		return (m_Type == a_Item.m_Type);
	}
	
	Type m_Type;
};

typedef F3DTreeNode<CircleItem> F3DCircleNode;
typedef F3DCircleNode::F3DTreeNodePtr F3DCircleNodePtr;

typedef std::vector<CircleItem> CircleVectorX;
typedef std::vector<CircleVectorX> CircleVectorY;
typedef std::vector<CircleVectorY> CircleVectorZ;

typedef std::shared_ptr<CircleVectorZ> ObjectDataPtr;

#pragma pack ( pop)