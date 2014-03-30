#include "Common.h"
#include "SolidObjectGenerator.h"
#include "ThreadPool/AsyncOpForThreadPool.h"
#include "ThreadPool/AsyncParallelOperation.h"

namespace
{
	class FillItemsOp : public ThreadPool::AsyncOpForPool
	{
	public:
		FillItemsOp(const F3DCircleNodePtr& a_InOutObject, const IntPoint& a_Center, const double& a_Radius, const CircleItem& a_Item, const ThreadPool::ThreadPoolPtr& a_ThreadPool)
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
		IntPoint m_Center;
		double m_Radius;
		CircleItem m_Item;
	};	
	
	static IntPoint GetCenter(const F3DCircleNodePtr& a_InOutObject)
	{
		const IntPoint& cur_point = a_InOutObject->GetCurPosition();
		const size_t sdvig = a_InOutObject->GetChildNodesCount() + 1;
		IntPoint cur_abs_point(cur_point.x << sdvig, cur_point.y << sdvig, cur_point.z << sdvig);
		
		const int ml = (int)a_InOutObject->GetLength() / 2;
		IntPoint midle_point(ml, ml, ml);
		
		return (cur_abs_point + midle_point);
	}
	
	static void FillItems(const F3DCircleNodePtr& a_InOutObject, const IntPoint& a_Center, const double& a_Radius, const CircleItem& a_Item)
	{
		IntPoint vec_center = GetCenter(a_InOutObject) - a_Center;
		
		double bb_size2 = 3.0 * a_InOutObject->GetLength() / 2.0 * a_InOutObject->GetLength() / 2.0;
		double radius2 = a_Radius * a_Radius;
		
		double distance_to_center2 = vec_center.x * vec_center.x + vec_center.y * vec_center.y + vec_center.z * vec_center.z;
		
		if (distance_to_center2 > (radius2 + bb_size2))
			return;

		F3DCircleNode::ItemVector& items = a_InOutObject->GetItems();
		if (distance_to_center2 + bb_size2 < radius2)
		{
			for (size_t i = 0; i < items.size(); ++i)
				items[i] = a_Item;			

			return;
		}
		
		a_InOutObject->CreateAllChild();

		const F3DCircleNode::NodeVector& nodes = a_InOutObject->GetChildNodes();
		
		if (!nodes.empty())
		{
			for (size_t i = 0; i < nodes.size(); ++i)
			{
				const F3DCircleNodePtr& cur_node = nodes[i];
				
				FillItems(cur_node, a_Center, a_Radius, a_Item);
			}
			return;
		}
		
		IntPoint cur_node_point = a_InOutObject->GetCurPosition() * 2;
		for (size_t i = 0; i < items.size(); ++i)
		{
			IntPoint cur_point = cur_node_point + IndexToPoiter(i);
			
			CircleItem& item = items[i];
			irr::core::vector3df center(a_Center.x + 0.5, a_Center.y + 0.5, a_Center.z + 0.5);
			irr::core::vector3df cur_vector(cur_point.x + 0.5, cur_point.y + 0.5, cur_point.z + 0.5);
			irr::core::vector3df cvector = cur_vector - center;
			if (cvector.getLengthSQ() < radius2)
				item = a_Item;
		}
	}
	
	void FillItemsOp::__SyncRun()
	{
		FillItems(m_InOutObject, m_Center, m_Radius, m_Item);
		m_InOutObject->Pack();
	}

}

ThreadPool::IAsyncOperationPtr OpFillItems(const F3DCircleNodePtr& a_InOutObject, const IntPoint& a_Center, const double& a_Radius, const CircleItem& a_Item, const size_t& a_GroupLength, const ThreadPool::ThreadPoolPtr& a_ThreadPool)
{
	if (a_InOutObject->GetLength() <= a_GroupLength)
		return std::make_shared<FillItemsOp>(std::cref(a_InOutObject), std::cref(a_Center), std::cref(a_Radius), std::cref(a_Item), std::cref(a_ThreadPool));

	a_InOutObject->CreateAllChild();

	const F3DCircleNode::NodeVector& nodes = a_InOutObject->GetChildNodes();
	
	if (nodes.empty())
		return std::make_shared<FillItemsOp>(std::cref(a_InOutObject), std::cref(a_Center), std::cref(a_Radius), std::cref(a_Item), std::cref(a_ThreadPool));

	ThreadPool::AsyncOpVector ops;
	for (size_t i = 0; i < nodes.size(); ++i)
	{
		const F3DCircleNodePtr& cur_node = nodes[i];
		
		ThreadPool::IAsyncOperationPtr op = OpFillItems(cur_node, a_Center, a_Radius, a_Item, a_GroupLength, a_ThreadPool);
		ops.push_back(op);
	}
	
	return std::make_shared<ThreadPool::AsyncParallelOperation>(ops);
}