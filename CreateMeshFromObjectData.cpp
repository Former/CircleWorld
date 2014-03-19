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
		irr::video::S3DVertex new_item(point, irr::core::vector3df(0 ,0, 0), a_Strategy->GetColor(a_Item, a_CurPoint), texture_index[i]);
		
		a_Buffer->Vertices.push_back(new_item);
	}
}

static IntPoint SummPoint(const IntPoint& a_Point1, const IntPoint& a_Point2)
{
	return IntPoint(a_Point1.x + a_Point2.x, a_Point1.y + a_Point2.y, a_Point1.z + a_Point2.z);
}

static bool CheckInsideVector(const IntPoint& a_Point, const IntPoint& a_MaxPoint)
{
	if (a_Point.x < 0 || a_Point.y < 0 || a_Point.z < 0)
		return false;

	if (a_Point.x >= a_MaxPoint.x || a_Point.y >= a_MaxPoint.y || a_Point.z >= a_MaxPoint.z)
		return false;

	return true;
}

int s_AddFaceCount = 0;

typedef std::vector< std::vector<irr::scene::SMeshBuffer*> > ObjectBufferVector;

static void CreateMeshItem(IN OUT ObjectBufferVector& a_ObjectBuffers, const CircleVectorZ& a_ObjectData, const ObjectDrawStrategyPtr& a_Strategy, const CircleItem& a_Item, const IntPoint& a_CurPoint, const IntPoint& a_MaxPoint, const double& a_Step, const size_t& a_DrawStep)
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
		
		if (CheckInsideVector(near_point, a_MaxPoint))
		{		
			if (!a_Strategy->AddFace(cur_index, near_point, a_DrawStep))
				continue;
		}
		
		if (!add_vertices)
			AddVerticesToMeshBuffer(buffer, a_Strategy, a_Item, a_CurPoint, a_MaxPoint, a_Step, a_DrawStep);		
		
		++s_AddFaceCount;
		AddMeshFace(buffer, vertices_count, point_numbers[i]);
	}
}

static ObjectBufferVector CreateMeshFromObjectDataItem(IN const IntPoint& a_StartPoint, IN const IntPoint& a_EndPoint, const CircleVectorZ& a_ObjectData, const ObjectDrawStrategyPtr& a_Strategy, const double& a_Step, const size_t& a_DrawStep)
{
	ObjectBufferVector object_buffers;
	
	for (size_t z = a_StartPoint.z; z < a_EndPoint.z; z += a_DrawStep)
	{
		const CircleVectorY& y_items = a_ObjectData[z];
		for (size_t y = a_StartPoint.y; y < a_EndPoint.y; y += a_DrawStep)
		{
			const CircleVectorX& x_items = y_items[y];
			for (size_t x = a_StartPoint.x; x < a_EndPoint.x; x += a_DrawStep)
			{
				const CircleItem& item = x_items[x];
				
				IntPoint cur_point = IntPoint(int(x), int(y), int(z));
				IntPoint max_point = IntPoint(int(x_items.size()), int(y_items.size()), int(a_ObjectData.size()));
				
				CreateMeshItem(object_buffers, a_ObjectData, a_Strategy, item, cur_point, max_point, a_Step, a_DrawStep);
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

typedef std::pair<IntPoint, IntPoint> StartEndItem;
typedef std::vector<std::pair<IntPoint, IntPoint>> StartEndVector;

static SMeshVector CreateMeshFromObjectDataWorker(IN const StartEndVector& a_Items, const CircleVectorZ& a_ObjectData, const ObjectDrawStrategyPtr& a_Strategy, const double& a_Step, const size_t& a_DrawStep)
{
	SMeshVector result;
	for (size_t i = 0; i < a_Items.size(); ++i)
	{
		const StartEndItem& item = a_Items[i];
		ObjectBufferVector cur_buf = CreateMeshFromObjectDataItem(item.first, item.second, a_ObjectData, a_Strategy, a_Step, a_DrawStep);
		
		result.push_back(CreateMeshFormObjectBuffers(cur_buf));
	}
	
	return result;
}

SMeshVector CreateMeshFromObjectData(const CircleVectorZ& a_ObjectData, const ObjectDrawStrategyPtr& a_Strategy, const double& a_Step, const size_t& a_DrawStep, const size_t& a_DivStep)
{	
	const size_t max_z = int(a_ObjectData.size());
	if (!max_z)
		return SMeshVector();
	const size_t max_y = int(a_ObjectData[0].size());
	if (!max_y)
		return SMeshVector();
	const size_t max_x = int(a_ObjectData[0][0].size());
	
	size_t num_of_cpu = sysconf(_SC_NPROCESSORS_ONLN);
	std::vector<StartEndVector> items(num_of_cpu);
	size_t cur_cpu = 0;
	
	for (size_t z = 0; z < max_z; z += a_DivStep)
	{
		for (size_t y = 0; y < max_y; y += a_DivStep)
		{
			for (size_t x = 0; x < max_x; x += a_DivStep)
			{
				IntPoint start(x + x % a_DrawStep, y + y % a_DrawStep, z + z % a_DrawStep);
				IntPoint end((std::min)(x + a_DivStep, max_x), (std::min)(y + a_DivStep, max_y), (std::min)(z + a_DivStep, max_z));
				
				items[cur_cpu].push_back(StartEndItem(start, end));
				cur_cpu++;
				if (cur_cpu >= num_of_cpu)
					cur_cpu = 0;
			}
		}
	}
	
	std::vector<std::future<SMeshVector>> worker_results;
	for (size_t i = 0; i < items.size(); ++i)
		worker_results.push_back(std::async(std::launch::async, CreateMeshFromObjectDataWorker, std::ref(items[i]), std::ref(a_ObjectData), std::ref(a_Strategy), std::ref(a_Step), std::ref(a_DrawStep)));
	
	SMeshVector result;
	for (size_t i = 0; i < worker_results.size(); ++i)
	{
		const SMeshVector& cur_tesult = worker_results[i].get();
		
		result.insert(result.end(), cur_tesult.begin(), cur_tesult.end());
	}
	
	return result;
}





















