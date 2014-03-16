#include "CreateMeshFromObjectData.h"
#include <tr1/memory>

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

static void Append(irr::scene::SMeshBuffer* a_InOut, const irr::scene::SMeshBuffer* const a_Other)
{
	if (a_InOut == a_Other)
		return;

	const irr::u32 vertexCount = a_InOut->getVertexCount();

	a_InOut->Vertices.reallocate(vertexCount+a_Other->getVertexCount());
	for (irr::u32 i=0; i<a_Other->getVertexCount(); ++i)
	{
		a_InOut->Vertices.push_back(reinterpret_cast<const irr::video::S3DVertex*>(a_Other->getVertices())[i]);
	}

	a_InOut->Indices.reallocate(a_InOut->getIndexCount()+a_Other->getIndexCount());
	for (irr::u32 i=0; i<a_Other->getIndexCount(); ++i)
	{
		a_InOut->Indices.push_back(a_Other->getIndices()[i]+vertexCount);
	}
	a_InOut->BoundingBox.addInternalBox(a_Other->getBoundingBox());
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
	
	const irr::video::SColor& color1 = a_Item.m_Color;
	const irr::video::SColor color[8]
	{
		irr::video::SColor(color1),
		irr::video::SColor(color1),
		irr::video::SColor(color1),
		irr::video::SColor(color1),
		irr::video::SColor(color1),
		irr::video::SColor(color1),
		irr::video::SColor(color1),
		irr::video::SColor(color1),
	};

	for (size_t i = 0; i < sizeof(points)/sizeof(points[0]); ++i)
	{
		irr::core::vector3df point = MakeCurPoint(a_CurPoint, a_MaxPoint, a_Step, points[i]);
		irr::video::S3DVertex new_item(point, irr::core::vector3df(0 ,0, 0), color[i], texture_index[i]);
		
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

irr::scene::SMesh* CreateMeshFromObjectData(const CircleVectorZ& a_ObjectData, const double& a_Step, const size_t& a_DrawStep)
{
	irr::scene::SMesh* mesh = new irr::scene::SMesh();
	irr::scene::SMeshBuffer* buffer = new irr::scene::SMeshBuffer;
	
	ObjectBufferVector object_buffers;
	
	size_t i = 0;
	for (size_t z = 0; z < a_ObjectData.size(); z += a_DrawStep)
	{
		const CircleVectorY& y_items = a_ObjectData[z];
		for (size_t y = 0; y < y_items.size(); y += a_DrawStep)
		{
			const CircleVectorX& x_items = y_items[y];
			for (size_t x = 0; x < x_items.size(); x += a_DrawStep)
			{
				const CircleItem& item = x_items[x];
				
				std::vector<int> cur_point = {int(x), int(y), int(z)};
				std::vector<int> max_point = {int(x_items.size()), int(y_items.size()), int(a_ObjectData.size())};
				
				CreateMeshItem(object_buffers, a_ObjectData, item, cur_point, max_point, a_Step, a_DrawStep);
			}
		}
	}
	
	for (size_t type = 0; type < object_buffers.size(); ++type)
	{
		const std::vector<irr::scene::SMeshBuffer*>& buffers = object_buffers[type];
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