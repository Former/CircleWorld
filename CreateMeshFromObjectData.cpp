#include "CreateMeshFromObjectData.h"

static void AddMeshFace(irr::scene::SMeshBuffer* a_MeshBuffer, const size_t& a_VerticesCount, const irr::u32 a_PointNumbers[4])
{
	const irr::u32 used[6] = {1, 3, 0, 2, 1, 0};
	for (irr::u32 i = 0; i < 6; ++i)
		a_MeshBuffer->Indices.push_back(a_VerticesCount + a_PointNumbers[used[i]]);
}

typedef std::shared_ptr<irr::video::SMaterial> SMaterialPtr;

static irr::core::vector3df MakeCurPoint(const IntPoint& a_CurPoint, const IntPoint& a_MaxPoint, const double& a_Step, const irr::core::vector3df& a_Diff)
{
	irr::core::vector3df result = a_Diff;
	result.X += (a_CurPoint.x * a_Step) - a_MaxPoint.x * a_Step * 0.5;
	result.Y += (a_CurPoint.y * a_Step) - a_MaxPoint.y * a_Step * 0.5;
	result.Z += (a_CurPoint.z * a_Step) - a_MaxPoint.z * a_Step * 0.5;
	
	return result;
}

static void AddVerticesToMeshBuffer(irr::scene::SMeshBuffer* a_Buffer, const ObjectDrawStrategyPtr& a_Strategy, const CircleItem& a_Item, const IntPoint& a_CurPoint, const IntPoint& a_MaxPoint, const double& a_Step, const size_t& a_DrawStep)
{
	const double& step = a_Step * a_DrawStep;
	const irr::core::vector3df points[8] = 
	{
		irr::core::vector3df(0, 0, 0),
		irr::core::vector3df(0, step, 0),
		irr::core::vector3df(step, 0, 0),
		irr::core::vector3df(step, step, 0),
		irr::core::vector3df(0, 0, step),
		irr::core::vector3df(0, step, step),
		irr::core::vector3df(step, 0, step),
		irr::core::vector3df(step, step, step),
	}; 

	const int tstep = a_DrawStep;	
	const irr::core::vector2d<irr::f32> texture_index[8] = 
	{
		irr::core::vector2d<irr::f32>(tstep, tstep),
		irr::core::vector2d<irr::f32>(0, 0),
		irr::core::vector2d<irr::f32>(0, tstep),
		irr::core::vector2d<irr::f32>(tstep, 0),
		irr::core::vector2d<irr::f32>(tstep, 0),
		irr::core::vector2d<irr::f32>(0, tstep),
		irr::core::vector2d<irr::f32>(0, 0),
		irr::core::vector2d<irr::f32>(tstep, tstep),
	};
	
	for (size_t i = 0; i < sizeof(points)/sizeof(points[0]); ++i)
	{
		irr::core::vector3df point = MakeCurPoint(a_CurPoint, a_MaxPoint, a_Step, points[i]);
		irr::video::S3DVertex new_item(point, irr::core::vector3df(0 ,0, 0), a_Strategy->GetColor(a_CurPoint, a_DrawStep), texture_index[i]);
		
		a_Buffer->Vertices.push_back(new_item);
	}
}

static IntPoint SummPoint(const IntPoint& a_Point1, const IntPoint& a_Point2)
{
	return IntPoint(a_Point1.x + a_Point2.x, a_Point1.y + a_Point2.y, a_Point1.z + a_Point2.z);
}

int s_AddFaceCount = 0;

typedef std::vector< std::vector<irr::scene::SMeshBuffer*> > ObjectBufferVector;

static void CreateMeshItem(IN OUT ObjectBufferVector& a_ObjectBuffers, const ObjectDrawStrategyPtr& a_Strategy, const IntPoint& a_CurPoint, const IntPoint& a_MaxPoint, const double& a_Step, const size_t& a_DrawStep)
{
	if (a_Strategy->IgnoreFace(a_CurPoint, a_DrawStep))
		return;
		
	const irr::u32 point_numbers[6][4] = 
	{
		{0, 3, 1, 2},
		{2, 7, 3, 6},
		{6, 5, 7, 4},
		{4, 1, 5, 0},
		{3, 5, 1, 7},
		{0, 6, 2, 4},
	};

	int step = a_DrawStep;
	const std::vector< IntPoint > near_item_index =
	{
		IntPoint(0, 0, -step),
		IntPoint(step, 0, 0),
		IntPoint(0, 0, step),
		IntPoint(-step, 0, 0),
		IntPoint(0, step, 0),
		IntPoint(0, -step, 0),		
	};
	
	if (a_ObjectBuffers.size() < CircleItem::tpCount)
		a_ObjectBuffers.resize(CircleItem::tpCount);

	const CircleItem& a_Item = a_Strategy->GetItem(a_CurPoint, a_DrawStep);
	const CircleItem::Type cur_type = a_Item.m_Type;
	std::vector<irr::scene::SMeshBuffer*>& buffers = a_ObjectBuffers[cur_type];
	if (buffers.empty() || buffers.back()->Indices.size() > 30000)
	{
		irr::scene::SMeshBuffer* new_buffer = new irr::scene::SMeshBuffer;
		buffers.push_back(new_buffer);
		new_buffer->Material = a_Strategy->GetMaterial(a_Item);
	}
	
	irr::scene::SMeshBuffer* buffer = buffers.back();
	const size_t vertices_count = buffer->Vertices.size();
	
	bool add_vertices = false;
	for (size_t i = 0; i < near_item_index.size(); ++i)
	{
		const IntPoint& cur_index = near_item_index[i];
 		IntPoint near_point = SummPoint(cur_index, a_CurPoint);
		
		if (!a_Strategy->AddFace(cur_index, near_point, a_DrawStep))
			continue;
		
		if (!add_vertices)
		{
			AddVerticesToMeshBuffer(buffer, a_Strategy, a_Item, a_CurPoint, a_MaxPoint, a_Step, a_DrawStep);
			add_vertices = true;
		}
		
		++s_AddFaceCount;
		AddMeshFace(buffer, vertices_count, point_numbers[i]);
	}
}

static ObjectBufferVector CreateMeshFromObjectDataItem(IN const IntPoint& a_StartPoint, IN const IntPoint& a_EndPoint, IN const IntPoint& a_MaxPoint, const ObjectDrawStrategyPtr& a_Strategy, const double& a_Step, const size_t& a_DrawStep)
{
	ObjectBufferVector object_buffers;
	
	for (size_t z = a_StartPoint.z; z < a_EndPoint.z; z += a_DrawStep)
	{
		for (size_t y = a_StartPoint.y; y < a_EndPoint.y; y += a_DrawStep)
		{
			for (size_t x = a_StartPoint.x; x < a_EndPoint.x; x += a_DrawStep)
			{
				IntPoint cur_point = IntPoint(int(x), int(y), int(z));
				
				CreateMeshItem(object_buffers, a_Strategy, cur_point, a_MaxPoint, a_Step, a_DrawStep);
			}
		}
	}
	
	return object_buffers;	
}

static irr::scene::SMesh* CreateMeshFormObjectBuffers(IN const ObjectBufferVector& a_ObjectBuffer)
{
	irr::scene::SMesh* mesh = new irr::scene::SMesh();
	
	for (size_t type = 0; type < a_ObjectBuffer.size(); ++type)
	{
		const std::vector<irr::scene::SMeshBuffer*>& buffers = a_ObjectBuffer[type];
		for (size_t i = 0; i < buffers.size(); ++i)
		{
			irr::scene::SMeshBuffer* buffer = buffers[i];			
			buffer->recalculateBoundingBox();
			if (!buffer->Vertices.empty())
				mesh->addMeshBuffer(buffer);
		}
	}
	
	mesh->setDirty();
	mesh->recalculateBoundingBox();
	mesh->setHardwareMappingHint(irr::scene::EHM_STATIC);
	
	return mesh;
}

SMeshDrawStepsVector CreateMeshFromObjectDataWorker(IN StartEndStepVector a_Items, ObjectDrawStrategyPtr a_Strategy, double a_Step, IN IntPoint a_MaxPoint)
{
	if (a_Items.empty())
		return SMeshDrawStepsVector();
	
	SMeshDrawStepsVector result;
	for (size_t i = 0; i < a_Items.size(); ++i)
	{
		const StartEndStepItem& item = a_Items[i];
		const StartEndVector& se_vector = item.first;
		size_t draw_step = item.second;
		result.resize(se_vector.size());
		for (size_t j = 0; j < se_vector.size(); ++j)
		{
			const StartEndItem& se_item = se_vector[j];
			ObjectBufferVector cur_buf = CreateMeshFromObjectDataItem(se_item.first, se_item.second, a_MaxPoint, a_Strategy, a_Step, draw_step);
		
			result[j].push_back(CreateMeshFormObjectBuffers(cur_buf));		
		}
	}
	
	return result;
}

StartEndVector MakeStartEndItems(IN const IntPoint& a_StartPoint, IN const IntPoint& a_EndPoint, const size_t& a_DrawStep, const size_t& a_DivStep)
{
	std::vector<StartEndItem> result;
	
	for (size_t z = a_StartPoint.z; z < a_EndPoint.z; z += a_DivStep)
	{
		for (size_t y = a_StartPoint.y; y < a_EndPoint.y; y += a_DivStep)
		{
			for (size_t x = a_StartPoint.x; x < a_EndPoint.x; x += a_DivStep)
			{
				IntPoint start(x + (x - a_StartPoint.x) % a_DrawStep, y + (y - a_StartPoint.y) % a_DrawStep, z + (z - a_StartPoint.z) % a_DrawStep);
				IntPoint end((std::min)(x + a_DivStep, (size_t)(a_EndPoint.x)), (std::min)(y + a_DivStep, (size_t)(a_EndPoint.y)), (std::min)(z + a_DivStep, (size_t)(a_EndPoint.z)));
				
				result.push_back(StartEndItem(start, end));
			}
		}
	}
	
	return result;
}

std::vector<StartEndVector> MakeStartEndVectorByGroup(IN const StartEndVector& a_StartEndVector, IN const size_t& a_GroupCount)
{
	std::vector<StartEndVector> result(a_GroupCount);
	size_t cur_group_number = 0;
	for (size_t i = 0; i < a_StartEndVector.size(); ++i)
	{
		result[cur_group_number].push_back(a_StartEndVector[i]);
		cur_group_number++;
		if (cur_group_number >= a_GroupCount)
			cur_group_number = 0;
	}
	
	return result;
}

SMeshVector CreateMeshFromObjectData(const CircleVectorZ& a_ObjectData, const ObjectDrawStrategyPtr& a_Strategy, const double& a_Step, const size_t& a_DrawStep, const size_t& a_DivStep)
{	
	const IntPoint end = a_Strategy->GetEndPoint();
	const IntPoint start = a_Strategy->GetStartPoint();
	
	size_t num_of_cpu = a_Strategy->GetThreadCount();
	std::vector<StartEndItem> div_item = MakeStartEndItems(start, end, a_DrawStep, a_DivStep);
	std::vector<StartEndVector> items = MakeStartEndVectorByGroup(div_item, num_of_cpu);
		
	std::vector<FutureMeshDrawStepsVector> worker_results;
	for (size_t i = 0; i < items.size(); ++i)
	{
		StartEndStepItem item(items[i], a_DrawStep);
		StartEndStepVector steps = {item};
		worker_results.push_back(std::async(std::launch::async, CreateMeshFromObjectDataWorker, std::ref(steps), std::ref(a_Strategy), std::ref(a_Step), std::ref(end)));
	}
	
	SMeshVector result;
	for (size_t i = 0; i < worker_results.size(); ++i)
	{
		const SMeshDrawStepsVector& cur_tesult = worker_results[i].get();
		
		result.insert(result.end(), cur_tesult[0].begin(), cur_tesult[0].end());
	}
	
	return result;
}

#include "ThreadPool/AsyncOpForThreadPool.h"
#include "ThreadPool/AsyncParallelOperation.h"
/*
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
*/
ThreadPool::IAsyncOperationPtr MakeDrawOp(const F3DCircleNodePtr& a_InOutObject, const ObjectDrawStrategyPtr& a_Strategy, const ThreadPool::ThreadPoolPtr& a_ThreadPool)
{
/*	if (a_InOutObject->GetLength() <= a_GroupLength)
		return std::make_shared<FillItemsOp>(std::cref(a_InOutObject), std::cref(a_Center), std::cref(a_Radius), std::cref(a_Item), std::cref(a_ThreadPool));

	a_InOutObject->CreateAllChild();

	const F3DCircleNode::NodeVector& nodes = a_InOutObject->GetChildNodes();
	
	if (nodes.empty())
		return std::make_shared<FillItemsOp>(std::cref(a_InOutObject), std::cref(a_Center), std::cref(a_Radius), std::cref(a_Item), std::cref(a_ThreadPool));

	for (size_t i = 0; i < nodes.size(); ++i)
	{
		const F3DCircleNodePtr& cur_node = nodes[i];
		
		ThreadPool::IAsyncOperationPtr op = OpFillItems(cur_node, a_Center, a_Radius, a_Item, a_GroupLength, a_ThreadPool);
		ops.push_back(op);
	}
*/	
	ThreadPool::AsyncOpVector ops;
	return std::make_shared<ThreadPool::AsyncParallelOperation>(ops);
}
















