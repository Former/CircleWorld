#include "Common.h"
#include "SolidObjectGenerator.h"
#include "ThreadPool/AsyncOpForThreadPool.h"
#include "ThreadPool/AsyncParallelOperation.h"

namespace
{
	class FillItemsOp : public ThreadPool::AsyncOpForPool
	{
	public:
		FillItemsOp(const F3DCircleNodePtr& a_InOutObject, const F3DCircleNode::Point& a_Center, const double& a_Radius, const CircleItem& a_Item, const ThreadPool::ThreadPoolPtr& a_ThreadPool)
		: ThreadPool::AsyncOpForPool(a_ThreadPool)
		{
			m_InOutObject = a_InOutObject;
			m_Center = a_Center;
			m_Radius = a_Radius;
			m_Item = a_Item;
		}

		virtual double GetPriority() const override
		{
			return 0;
		}
	
	protected:
		virtual void __SyncRun() override;
		
	private:
		F3DCircleNodePtr m_InOutObject;
		F3DCircleNode::Point m_Center;
		double m_Radius;
		CircleItem m_Item;
	};	
	
	static void FillItems(const F3DCircleNodePtr& a_InOutObject, const F3DCircleNode::Point& a_Center, const double& a_Radius, const CircleItem& a_Item)
	{
		F3DCircleNode::TreeBBox bbox = a_InOutObject->GetBBox();
		F3DCircleNode::Point vec_center = bbox.GetCenter() - a_Center;
		
		double bb_size = bbox.GetRadius();
		double distance_to_center = vec_center.GetLength();
		
		if (distance_to_center > (a_Radius + bb_size))
			return;

		CircleItem* items = a_InOutObject->GetItems();
		if ((distance_to_center + bb_size) < a_Radius)
		{
			for (size_t i = 0; i < a_InOutObject->GetItemsCount(); ++i)
				items[i] = a_Item;			

			return;
		}
		
		a_InOutObject->CreateAllChild();

		const F3DCircleNode::F3DTreeNodePtr* nodes = a_InOutObject->GetChildNodes();
		const size_t nodes_count = a_InOutObject->GetChildNodesCount();
		
		if (nodes_count)
		{
			for (size_t i = 0; i < nodes_count; ++i)
			{
				const F3DCircleNodePtr& cur_node = nodes[i];
				
				FillItems(cur_node, a_Center, a_Radius, a_Item);
			}
			return;
		}
		
		F3DCircleNode::Point cur_node_point = a_InOutObject->GetCurPosition() * static_cast<F3DCircleNode::CoordTreeType>(2);
		for (size_t i = 0; i < a_InOutObject->GetItemsCount(); ++i)
		{
			F3DCircleNode::Point cur_point = cur_node_point + IndexToPoiter(i);
			
			CircleItem& item = items[i];
			irr::core::vector3df center(a_Center.x + 0.5, a_Center.y + 0.5, a_Center.z + 0.5);
			irr::core::vector3df cur_vector(cur_point.x + 0.5, cur_point.y + 0.5, cur_point.z + 0.5);
			irr::core::vector3df cvector = cur_vector - center;
			if (cvector.getLengthSQ() < a_Radius * a_Radius)
				item = a_Item;
		}
	}
	
	void FillItemsOp::__SyncRun()
	{
		FillItems(m_InOutObject, m_Center, m_Radius, m_Item);
		m_InOutObject->Pack();
	}

}

ThreadPool::IAsyncOperationPtr OpFillItems(const F3DCircleNodePtr& a_InOutObject, const F3DCircleNode::Point& a_Center, const double& a_Radius, const CircleItem& a_Item, const size_t& a_GroupLength, const ThreadPool::ThreadPoolPtr& a_ThreadPool)
{
	if (a_InOutObject->GetLength() <= a_GroupLength)
		return std::make_shared<FillItemsOp>(std::cref(a_InOutObject), std::cref(a_Center), std::cref(a_Radius), std::cref(a_Item), std::cref(a_ThreadPool));

	a_InOutObject->CreateAllChild();

	const F3DCircleNode::F3DTreeNodePtr* nodes = a_InOutObject->GetChildNodes();
	const size_t nodes_count = a_InOutObject->GetChildNodesCount();
	
	if (!nodes_count)
		return std::make_shared<FillItemsOp>(std::cref(a_InOutObject), std::cref(a_Center), std::cref(a_Radius), std::cref(a_Item), std::cref(a_ThreadPool));

	ThreadPool::AsyncOpVector ops;
	for (size_t i = 0; i < nodes_count; ++i)
	{
		const F3DCircleNodePtr& cur_node = nodes[i];
		
		ThreadPool::IAsyncOperationPtr op = OpFillItems(cur_node, a_Center, a_Radius, a_Item, a_GroupLength, a_ThreadPool);
		ops.push_back(op);
	}
	
	return std::make_shared<ThreadPool::AsyncParallelOperation>(ops);
}