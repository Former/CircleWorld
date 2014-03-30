#include <gtest/gtest.h>

#include <F3DTree.h>

namespace
{
	class TestItem
	{
	public:
		TestItem();
		TestItem(const int& a_Value);
		TestItem(const std::vector<TestItem>& a_Items);
	
		bool operator == (const TestItem& a_Value) const;
	
		int GetValue() const;
		
	private:
		int m_Value;
	};

	TestItem::TestItem()
	{
		m_Value = 0;
	}

	TestItem::TestItem(const int& a_Value)
	{
		m_Value = a_Value;
	}
	
	TestItem::TestItem(const std::vector<TestItem>& a_Items)
	{
		size_t summ = 0;
		for (size_t i = 0; i < a_Items.size(); ++i)
			summ += a_Items[i].m_Value;

		m_Value = summ;
	}
	
	int TestItem::GetValue() const
	{
		return m_Value;
	}
	
	bool TestItem::operator == (const TestItem& a_Value) const
	{
		return (GetValue() == a_Value.GetValue());
	}
}

typedef std::vector<TestItem> ItemsVector;
typedef F3DTreeNode<TestItem> F3DTreeTest;

TEST(F3DTree, Init)
{
	F3DTreeTest::F3DTreeNodePtr tree = F3DTreeTest::CreateTree(TestItem(1), 5);
	
	EXPECT_EQ(5, tree->GetChildNodesCount());
	EXPECT_EQ(true, !tree->GetParent());
	EXPECT_EQ(0, tree->GetChildNodes().size());
	
	EXPECT_EQ(true, tree->GetCurPosition() == IntPoint(0, 0, 0));
	
	
	const ItemsVector& items = tree->GetItems();
	
	EXPECT_EQ(8, items.size());
	for (size_t i = 0; i < items.size(); ++i)
	{
		EXPECT_EQ(true, items[0] == items[0]);
	}

	EXPECT_EQ(2 * 2 * 2 * 2 * 2, tree->GetLength());
}

TEST(F3DTree, SetItem)
{
	F3DTreeTest::F3DTreeNodePtr tree = F3DTreeTest::CreateTree(TestItem(0), 1);
	
	tree->SetItem(TestItem(1), IntPoint(2, 2, 2));
	tree->SetItem(TestItem(1), IntPoint(1, 1, 1));
	
	EXPECT_EQ(8, tree->GetChildNodes().size());
}

TEST(F3DTree, GetItem)
{
	F3DTreeTest::F3DTreeNodePtr tree = F3DTreeTest::CreateTree(TestItem(0), 2);
	
	tree->SetItem(TestItem(1), IntPoint(0, 1, 2));
	tree->SetItem(TestItem(2), IntPoint(2, 1, 0));

	F3DTreeTest::ItemWithNode item1 = tree->GetItemWithNode(IntPoint(0, 1, 2));
	F3DTreeTest::ItemWithNode item2 = tree->GetItemWithNode(IntPoint(2, 1, 0));

	F3DTreeTest::ItemWithNode item0 = tree->GetItemWithNode(IntPoint(7, 7, 7));
	
	EXPECT_EQ(1, item1.m_Item.GetValue());
	ASSERT_EQ(false, !item1.m_Node);
	EXPECT_EQ(0, item1.m_Node->GetChildNodesCount());
	EXPECT_EQ(true, item1.m_Node->GetCurPosition() == IntPoint(0, 0, 1));


	EXPECT_EQ(2, item2.m_Item.GetValue());
	ASSERT_EQ(false, !item2.m_Node);
	EXPECT_EQ(0, item2.m_Node->GetChildNodesCount());
	EXPECT_EQ(true, item2.m_Node->GetCurPosition() == IntPoint(1, 0, 0));

	EXPECT_EQ(0, item0.m_Item.GetValue());
	ASSERT_EQ(false, !item0.m_Node);
	EXPECT_EQ(2, item0.m_Node->GetChildNodesCount());
	EXPECT_EQ(true, item0.m_Node->GetCurPosition() == IntPoint(0, 0, 0));
}

TEST(F3DTree, summItem)
{
	F3DTreeTest::F3DTreeNodePtr tree = F3DTreeTest::CreateTree(TestItem(0), 31);
	
	tree->SetItem(TestItem(1), IntPoint(1, 1, 1));
	tree->SetItem(TestItem(2), IntPoint(2, 2, 2));

	F3DTreeTest::ItemWithNode item0 = tree->GetItemWithNode(IntPoint(0, 0, 0), 3);
	
	EXPECT_EQ(3, item0.m_Item.GetValue());
}

TEST(F3DTree, Pack)
{
	F3DTreeTest::F3DTreeNodePtr tree = F3DTreeTest::CreateTree(TestItem(0), 10);
	
	tree->SetItem(TestItem(0), IntPoint(0, 0, 0));

	EXPECT_EQ(8, tree->GetChildNodes().size());

	tree->Pack();

	EXPECT_EQ(0, tree->GetChildNodes().size());
}
/*
TEST(F3DTree, PackOne)
{
	F3DTreeTest::F3DTreeNodePtr tree = F3DTreeTest::CreateTree(TestItem(0), 10);
	
	tree->SetItem(TestItem(0), IntPoint(0, 0, 0), 9);

	ASSERT_EQ(8, tree->GetChildNodes().size());

	for (size_t i = 0; i < tree->GetChildNodes().size(); ++i)
	{
		F3DTreeTest::F3DTreeNodePtr cur_node = tree->GetChildNodes()[i];
		if (cur_node)
			cur_node->PackOne();
	}
	tree->PackOne();

	EXPECT_EQ(0, tree->GetChildNodes().size());
}*/