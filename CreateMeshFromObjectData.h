#pragma once

#include "Common.h"
#include "IntPoint.h"
#include "CircleItem.h"
#include "ThreadPool/AsyncOperation.h"
#include "ThreadPool/ThreadPool.h"


class ObjectDrawStrategy
{
public:
	virtual ~ObjectDrawStrategy() {}
	
	virtual irr::video::SColor GetColor(const IntPoint& a_CurPoint, const size_t& a_DrawStep) = 0;
	
	virtual irr::video::SMaterial GetMaterial(const CircleItem& a_Item) = 0;

	virtual const CircleItem& GetItem(const IntPoint& a_CurPoint, const size_t& a_DrawStep) = 0;
	
	virtual bool AddFace(const IntPoint& a_CurPoint, const IntPoint& a_NearPoint, const size_t& a_DrawStep) = 0;
	
	virtual bool IgnoreFace(const IntPoint& a_CurPoint, const size_t& a_DrawStep) = 0;
	
	virtual size_t GetThreadCount() = 0;

	virtual double GetStep() = 0;
	
	virtual IntPoint GetStartPoint() = 0;

	virtual IntPoint GetEndPoint() = 0;
};

typedef std::shared_ptr<ObjectDrawStrategy> ObjectDrawStrategyPtr;

typedef std::vector<irr::scene::SMesh*> SMeshVector;

typedef std::pair<IntPoint, IntPoint> StartEndItem;
typedef std::vector<StartEndItem> StartEndVector;

typedef std::vector<SMeshVector> SMeshDrawStepsVector;
typedef std::future<SMeshDrawStepsVector> FutureMeshDrawStepsVector;
typedef std::pair<StartEndVector, size_t> StartEndStepItem;
typedef std::vector<StartEndStepItem> StartEndStepVector;

SMeshDrawStepsVector CreateMeshFromObjectDataWorker(IN StartEndStepVector a_Items, ObjectDrawStrategyPtr a_Strategy, double a_Step, IN IntPoint a_MaxPoint);

StartEndVector MakeStartEndItems(IN const IntPoint& a_StartPoint, IN const IntPoint& a_EndPoint, const size_t& a_DrawStep, const size_t& a_DivStep);

std::vector<StartEndVector> MakeStartEndVectorByGroup(IN const StartEndVector& a_StartEndVector, IN const size_t& a_GroupCount);

SMeshVector CreateMeshFromObjectData(const CircleVectorZ& a_ObjectData, const ObjectDrawStrategyPtr& a_Strategy, const double& a_Step, const size_t& a_DrawStep = 1, const size_t& a_DivStep = 1024);

///////////////////////////////////////////////////////


class DrawObject
{
public:
	DrawObject();
	
	irr::scene::SMesh* GetMesh() const;
	void SetMesh(irr::scene::SMesh* a_Mesh);
	
private:	
	irr::scene::SMesh* m_Mesh;
	std::mutex m_Mutex;
};

typedef std::shared_ptr<DrawObject> DrawObjectPtr;

class IDrawStrategy
{
public:
	virtual ~IDrawStrategy() {}
	
	virtual irr::video::SColor GetColor(const CircleItem& a_Item, const F3DCircleNodePtr& a_Node) = 0;
	
	virtual irr::video::SMaterial GetMaterial(const CircleItem& a_Item, const F3DCircleNodePtr& a_Node) = 0;

	virtual bool AddFace(const CircleItem& a_Item, const F3DCircleNodePtr& a_Node, const CircleItem& a_NearItem, const F3DCircleNodePtr& a_NearNode) = 0;
	
	virtual bool IgnoreFace(const CircleItem& a_Item, const F3DCircleNodePtr& a_Node) = 0;
	
	virtual double GetStep() = 0;
	
	virtual IntPoint GetMeshNodePosition() = 0;

	virtual size_t GetMeshNodeIndex() = 0;

	virtual size_t GetDrawNodeIndex() = 0;
};

typedef std::shared_ptr<IDrawStrategy> IDrawStrategyPtr;

class IPriorityCalculator
{
public:
	virtual ~IPriorityCalculator();
	
	virtual double GetPriority() = 0;
};

typedef std::shared_ptr<IPriorityCalculator> IPriorityCalculatorPtr;

ThreadPool::IAsyncOperationPtr MakeDrawOp(OUT const DrawObjectPtr& a_DrawObject, const F3DCircleNodePtr& a_Object, const IDrawStrategyPtr& a_Strategy, const IPriorityCalculatorPtr& a_PriorityCalculator, const ThreadPool::ThreadPoolPtr& a_ThreadPool);













