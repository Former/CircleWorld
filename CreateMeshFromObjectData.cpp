#include "CreateMeshFromObjectData.h"
#include <tr1/memory>
#include <future>

#define IN
#define OUT

static void AddMeshFace(irr::scene::SMeshBuffer* a_MeshBuffer, const size_t& a_VerticesCount, const irr::u32 a_PointNumbers[4])
{
	const irr::u32 used[6] = {1, 3, 0, 2, 1, 0};
	for (irr::u32 i = 0; i < 6; ++i)
		a_MeshBuffer->Indices.push_back(a_VerticesCount + a_PointNumbers[used[i]]);
}

typedef std::shared_ptr<irr::video::SMaterial> SMaterialPtr;

static irr::core::vector3df MakeCurPoint(const std::vector<int>& a_CurPoint, const std::vector<int>& a_MaxPoint, const double& a_Step, const irr::core::vector3df& a_Diff)
{
	irr::core::vector3df result = a_Diff;
	result.X += (a_CurPoint[0] * a_Step) - a_MaxPoint[0] * a_Step * 0.5;
	result.Y += (a_CurPoint[1] * a_Step) - a_MaxPoint[1] * a_Step * 0.5;
	result.Z += (a_CurPoint[2] * a_Step) - a_MaxPoint[2] * a_Step * 0.5;
	
	return result;
}

static void AddVerticesToMeshBuffer(irr::scene::SMeshBuffer* a_Buffer, const CircleItem& a_Item, const std::vector<int>& a_CurPoint, const std::vector<int>& a_MaxPoint, const double& a_Step, const size_t& a_DrawStep)
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

	const irr::core::vector2d<irr::f32> texture_index[8] = 
	{
		irr::core::vector2d<irr::f32>(1, 1),
		irr::core::vector2d<irr::f32>(0, 0),
		irr::core::vector2d<irr::f32>(0, 1),
		irr::core::vector2d<irr::f32>(1, 0),
		irr::core::vector2d<irr::f32>(1, 1),
		irr::core::vector2d<irr::f32>(0, 0),
		irr::core::vector2d<irr::f32>(0, 1),
		irr::core::vector2d<irr::f32>(1, 0),
	};
	
	for (size_t i = 0; i < sizeof(points)/sizeof(points[0]); ++i)
	{
		irr::core::vector3df point = MakeCurPoint(a_CurPoint, a_MaxPoint, a_Step, points[i]);
		irr::video::S3DVertex new_item(point, irr::core::vector3df(0 ,0, 0), a_Item.m_Color, texture_index[i]);
		
		a_Buffer->Vertices.push_back(new_item);
	}
}

static std::vector<int> SummVector(const std::vector<int>& a_Point1, const std::vector<int>& a_Point2)
{
	std::vector<int> result = a_Point1;
	for (size_t i = 0; i < result.size() && i < a_Point2.size(); ++i)
		result[i] += a_Point2[i];

	return result;
}

static bool CheckInsideVector(const std::vector<int>& a_Point, const std::vector<int>& a_MaxPoint)
{
	for (size_t i = 0; i < a_Point.size() && i < a_MaxPoint.size(); ++i)
	{
		const int& cur_index = a_Point[i];
		const int& cur_max_index = a_MaxPoint[i];
		if (cur_index < 0)
			return false;

		if (cur_index >= cur_max_index)
			return false;
	}

	return true;
}

int s_AddFaceCount = 0;

typedef std::vector< std::vector<irr::scene::SMeshBuffer*> > ObjectBufferVector;

static void CreateMeshItem(IN OUT ObjectBufferVector& a_ObjectBuffers, const CircleVectorZ& a_ObjectData, const CircleItem& a_Item, const std::vector<int>& a_CurPoint, const std::vector<int>& a_MaxPoint, const double& a_Step, const size_t& a_DrawStep)
{
	if (a_Item.m_Type == CircleItem::tpNone)
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
	const std::vector< std::vector<int> > near_item_index =
	{
		{0, 0, -step},
		{step, 0, 0},
		{0, 0, step},
		{-step, 0, 0},
		{0, step, 0},
		{0, -step, 0},		
	};
	
	if (a_ObjectBuffers.size() < CircleItem::tpCount)
		a_ObjectBuffers.resize(CircleItem::tpCount);

	const CircleItem::Type cur_type = a_Item.m_Type;
	std::vector<irr::scene::SMeshBuffer*>& buffers = a_ObjectBuffers[cur_type];
	if (buffers.empty() || buffers.back()->Indices.size() > 30000)
		buffers.push_back(new irr::scene::SMeshBuffer);
	
	irr::scene::SMeshBuffer* buffer = buffers.back();
	const size_t vertices_count = buffer->Vertices.size();
	
	bool add_vertices = false;
	for (size_t i = 0; i < near_item_index.size(); ++i)
	{
		const std::vector<int>& cur_index = near_item_index[i];
 		std::vector<int> near_item_index = SummVector(cur_index, a_CurPoint);
		
		if (CheckInsideVector(near_item_index, a_MaxPoint))
		{		
			const CircleItem& near_item = a_ObjectData[near_item_index[2]][near_item_index[1]][near_item_index[0]];
			if (near_item.m_Type != CircleItem::tpNone)
				continue;
		}
		
		if (!add_vertices)
			AddVerticesToMeshBuffer(buffer, a_Item, a_CurPoint, a_MaxPoint, a_Step, a_DrawStep);		
		
		++s_AddFaceCount;
		AddMeshFace(buffer, vertices_count, point_numbers[i]);
	}
}

namespace
{
	struct Point
	{
		Point(const int& a_X, const int& a_Y, const int& a_Z);
		
		int x;
		int y;
		int z;
	};
	
	Point::Point(const int& a_X, const int& a_Y, const int& a_Z)
	{
		x = a_X;
		y = a_Y;
		z = a_Z;
	}
}

static ObjectBufferVector CreateMeshFromObjectDataItem(IN const Point& a_StartPoint, IN const Point& a_EndPoint, const CircleVectorZ& a_ObjectData, const double& a_Step, const size_t& a_DrawStep)
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
				
				std::vector<int> cur_point = {int(x), int(y), int(z)};
				std::vector<int> max_point = {int(x_items.size()), int(y_items.size()), int(a_ObjectData.size())};
				
				CreateMeshItem(object_buffers, a_ObjectData, item, cur_point, max_point, a_Step, a_DrawStep);
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
			if (!buffer->Vertices.empty())
				mesh->addMeshBuffer(buffer);
		}
	}
	
	mesh->setDirty();
	mesh->recalculateBoundingBox();
	mesh->setHardwareMappingHint(irr::scene::EHM_STATIC);
	
	return mesh;
}

typedef std::pair<Point, Point> StartEndItem;
typedef std::vector<std::pair<Point, Point>> StartEndVector;

static SMeshVector CreateMeshFromObjectDataWorker(IN const StartEndVector& a_Items, const CircleVectorZ& a_ObjectData, const double& a_Step, const size_t& a_DrawStep)
{
	SMeshVector result;
	for (size_t i = 0; i < a_Items.size(); ++i)
	{
		const StartEndItem& item = a_Items[i];
		ObjectBufferVector cur_buf = CreateMeshFromObjectDataItem(item.first, item.second, a_ObjectData, a_Step, a_DrawStep);
		
		result.push_back(CreateMeshFormObjectBuffers(cur_buf));
	}
	
	return result;
}

SMeshVector CreateMeshFromObjectData(const CircleVectorZ& a_ObjectData, const double& a_Step, const size_t& a_DrawStep, const size_t& a_DivStep)
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
				Point start(x + x % a_DrawStep, y + y % a_DrawStep, z + z % a_DrawStep);
				Point end((std::min)(x + a_DivStep, max_x), (std::min)(y + a_DivStep, max_y), (std::min)(z + a_DivStep, max_z));
				
				items[cur_cpu].push_back(StartEndItem(start, end));
				cur_cpu++;
				if (cur_cpu >= num_of_cpu)
					cur_cpu = 0;
			}
		}
	}
	
	std::vector<std::future<SMeshVector>> worker_results;
	for (size_t i = 0; i < items.size(); ++i)
		worker_results.push_back(std::async(std::launch::async, CreateMeshFromObjectDataWorker, std::ref(items[i]), std::ref(a_ObjectData), std::ref(a_Step), std::ref(a_DrawStep)));
	
	SMeshVector result;
	for (size_t i = 0; i < worker_results.size(); ++i)
	{
		const SMeshVector& cur_tesult = worker_results[i].get();
		
		result.insert(result.end(), cur_tesult.begin(), cur_tesult.end());
	}
	
	return result;
}


