#include <irrlicht.h>
#include "driverChoice.h"

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

#include <string>
#include <sstream>

#include <thread>
#include <mutex>

#include "CircleCoordinator.h"
#include "GetWorkTime.h"
#include "Import_Obj.h"

bool g_NeedExit = false;
std::mutex g_GuiMutex;
size_t s_PhysCount = 0;
GetWorkTime	g_WorkTime;

void PhysicsThread(CircleCoordinator& a_Coordinator)
{
	try
	{
		while (!g_NeedExit)
		{
			std::chrono::milliseconds sleep_delay(1);
			std::this_thread::sleep_for(sleep_delay);
			std::lock_guard<std::mutex> lock(g_GuiMutex);
			
			a_Coordinator.DoStep();
			s_PhysCount++;
		}
	}
	catch(...)
	{
		return;
	}
}

CircleCoordinator::ObjectPtr CreateNewObject()
{
	CircleCoordinator::ObjectPtr objContainer(new CircleCoordinator::Object);
	CircleEngine::CircleObjectPtr obj(new CircleEngine::CircleObject);
	objContainer->Obj = obj;
	
	return objContainer;
}

class SystemEventReceiver : public irr::IEventReceiver
{
public:

	SystemEventReceiver(irr::scene::ISceneNode* skydome, CircleCoordinator& a_Coordinator) :
		Skydome(skydome), m_Coordinator(a_Coordinator)
	{
		Skydome->setVisible(true);
	}

	bool OnEvent(const irr::SEvent& event)
	{
		std::vector<std::pair<size_t, size_t> > well_index;
		well_index.push_back(std::pair<size_t, size_t>(5, 6));
		well_index.push_back(std::pair<size_t, size_t>(57, 58));
		well_index.push_back(std::pair<size_t, size_t>(116, 118));
		well_index.push_back(std::pair<size_t, size_t>(145, 147));
		static int index = 145; // 5, 57, 116(118), 145(147)
		static double dir = 1.0;

		if (event.EventType == irr::EET_KEY_INPUT_EVENT) //  && !event.KeyInput.PressedDown
		{
			switch (event.KeyInput.Key)
			{
			case irr::KEY_KEY_R:
				{
					for (size_t i = 0; i < well_index.size(); ++i)
					{
						const std::pair<size_t, size_t>& cur_pair = well_index[i];
						
						std::vector<CircleCoordinator::ObjectPtr> objectConts = m_Coordinator.GetObjects();
						CircleEngine::Point difCenter = (objectConts[cur_pair.first]->Obj->Center - objectConts[cur_pair.second]->Obj->Center);
						difCenter = difCenter / difCenter.Distance(CircleEngine::Point(0,0,0));
						objectConts[cur_pair.first]->Obj->Velocity =  difCenter * 2500.0 * dir;
					}
				}
				break;
			default:
				break;
			}
		}

		return false;
	}

private:
	irr::scene::ISceneNode* Skydome;
	CircleCoordinator& m_Coordinator;
};

void OnDisplay(CircleCoordinator& a_Coordinator)
{
	std::vector<CircleCoordinator::ObjectPtr> objects;
	{
		std::lock_guard<std::mutex> lock(g_GuiMutex);

		objects = a_Coordinator.GetObjects();
	}

	for (size_t i = 0; i < objects.size(); ++i)
	{
		CircleCoordinator::ObjectPtr obj = objects[i];
		CircleEngine::CircleObjectPtr circle_obj = obj->Obj;
		
		CircleEngine::Point draw_point = circle_obj->Center * a_Coordinator.m_Scale;
		if (obj->IrrObject)
			obj->IrrObject->setPosition(irr::core::vector3df(draw_point.x, draw_point.y, draw_point.z));
	}
}

struct CircleItem
{
	CircleItem()
	{
		m_Type = tpNone;
	}
	
	enum Type
	{
		tpNone,
		tpSolid,
	};
	
	Type m_Type;
	irr::video::SColor m_Color;
};

typedef std::vector<CircleItem> CircleVectorX;
typedef std::vector<CircleVectorX> CircleVectorY;
typedef std::vector<CircleVectorY> CircleVectorZ;

static void AddMeshFace(irr::scene::SMeshBuffer* a_MeshBuffer, const irr::u32 a_PointNumbers[4])
{
	const irr::u32 used[6] = {1, 3, 0, 2, 1, 0}; // {0, 1, 2, 0, 3, 1}; // 
	for (irr::u32 i = 0; i < 6; ++i)
		a_MeshBuffer->Indices.push_back(a_PointNumbers[used[i]]);
}

typedef std::shared_ptr<irr::video::SMaterial> SMaterialPtr;

static SMaterialPtr GetMaterialFromType(const CircleItem::Type& a_Type)
{
	static SMaterialPtr result;
	if (!result)
	{
		result = SMaterialPtr(new irr::video::SMaterial);
		//result->setTexture(0, driver->getTexture("../../media/wall.jpg"))
	}
	
	return result;
}

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

static irr::scene::SMeshBuffer* CreateMeshBuffer(const CircleItem& a_Item, const std::vector<int>& a_CurPoint, const std::vector<int>& a_MaxPoint, const double& a_Step)
{
	irr::scene::SMeshBuffer* buffer = new irr::scene::SMeshBuffer();
	
	const double& step = a_Step;
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

	buffer->Vertices.set_used(0);
	for (size_t i = 0; i < sizeof(points)/sizeof(points[0]); ++i)
	{
		irr::core::vector3df point = MakeCurPoint(a_CurPoint, a_MaxPoint, a_Step, points[i]);
		irr::video::S3DVertex new_item(point, irr::core::vector3df(0 ,0, 0), color[i], texture_index[i]);
		
		buffer->Vertices.push_back(new_item);
	}

	//buffer->Material = *GetMaterialFromType(a_Item.m_Type);
	
	return buffer;
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

static int s_AddFaceCount = 0;

static irr::scene::SMeshBuffer* CreateMeshItem(const CircleVectorZ& a_ObjectData, const CircleItem& a_Item, const std::vector<int>& a_CurPoint, const std::vector<int>& a_MaxPoint, const double& a_Step)
{
	if (a_Item.m_Type == CircleItem::tpNone)
		return 0;
		
	irr::scene::SMeshBuffer* buffer = 0;
	
	const irr::u32 point_numbers[6][4] = 
	{
		{0, 3, 1, 2},
		{2, 7, 3, 6},
		{6, 5, 7, 4},
		{4, 1, 5, 0},
		{3, 5, 1, 7},
		{0, 6, 2, 4},
	};

	const std::vector< std::vector<int> > near_item_index =
	{
		{0, 0, -1},
		{1, 0, 0},
		{0, 0, 1},
		{-1, 0, 0},
		{0, 1, 0},
		{0, -1, 0},		
	};

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
		
		if (!buffer)
			buffer = CreateMeshBuffer(a_Item, a_CurPoint, a_MaxPoint, a_Step);
		
		++s_AddFaceCount;
		AddMeshFace(buffer, point_numbers[i]);
	}

	if (buffer)
		buffer->setDirty();
	
	return buffer;
}

irr::scene::SMesh* CreateMeshFromObjectData(const CircleVectorZ& a_ObjectData, const double& a_Step)
{
	irr::scene::SMesh* mesh = new irr::scene::SMesh();
	irr::scene::SMeshBuffer* buffer = new irr::scene::SMeshBuffer;
	
	for (size_t z = 0; z < a_ObjectData.size(); ++z)
	{
		const CircleVectorY& y_items = a_ObjectData[z];
		for (size_t y = 0; y < y_items.size(); ++y)
		{
			const CircleVectorX& x_items = y_items[y];
			for (size_t x = 0; x < x_items.size(); ++x)
			{
				const CircleItem& item = x_items[x];
				
				std::vector<int> cur_point = {int(x), int(y), int(z)};
				std::vector<int> max_point = {int(x_items.size()), int(y_items.size()), int(a_ObjectData.size())};
				
				irr::scene::SMeshBuffer* new_buffer = CreateMeshItem(a_ObjectData, item, cur_point, max_point, a_Step);
				if (new_buffer)
					Append(buffer, new_buffer);
					//mesh->addMeshBuffer(new_buffer);
			}
		}
	}
	
	buffer->setDirty();
	mesh->addMeshBuffer(buffer);
	mesh->recalculateBoundingBox();
	return mesh;
}

irr::scene::SMeshBuffer* CreateMeshScuare(irr::core::vector3df a_Point, const irr::video::SMaterial& a_Material)
{
	irr::scene::SMeshBuffer* buffer = new irr::scene::SMeshBuffer();

	irr::u32 u[6] = {0, 1, 2, 0, 3, 1};
	buffer->Indices.set_used(6);
	for (irr::u32 i = 0; i < 6; ++i)
		buffer->Indices[i] = u[i];

	buffer->Vertices.set_used(4);

	double SIZEh = 500.0;
	
	irr::video::SColor clr(255,255,255,255);
	irr::video::SColor clr1(255,0,0,255);
	
	buffer->Vertices[0] = irr::video::S3DVertex(a_Point.X-SIZEh, a_Point.Y+SIZEh, a_Point.Z-SIZEh, 0, 0, 1, clr, 1, 1);
	buffer->Vertices[1] = irr::video::S3DVertex(a_Point.X+SIZEh, a_Point.Y+SIZEh, a_Point.Z+SIZEh, 0, 0, 1, clr1, 0, 0);
	buffer->Vertices[2] = irr::video::S3DVertex(a_Point.X+SIZEh, a_Point.Y+SIZEh, a_Point.Z-SIZEh, 0, 0, 1, clr1, 0, 1);
	buffer->Vertices[3] = irr::video::S3DVertex(a_Point.X-SIZEh, a_Point.Y+SIZEh, a_Point.Z+SIZEh, 0, 0, 1, clr, 1, 0);

	buffer->Material = a_Material;
	buffer->setDirty();
	
	return buffer;
}

int main()
{
	CircleCoordinator circle_coordinator;
	
	irr::SIrrlichtCreationParameters params;
	params.DriverType = irr::video::EDT_OPENGL;
	params.WindowSize = irr::core::dimension2d<irr::u32>(1024, 768);
	irr::IrrlichtDevice* device = irr::createDeviceEx(params);

	if (device == 0)
	{
		std::cout << "Could not create selected driver" << std::endl;
		return 1;
	}
	
	irr::video::IVideoDriver* driver = device->getVideoDriver();
	irr::scene::ISceneManager* smgr = device->getSceneManager();
	irr::gui::IGUIEnvironment* env = device->getGUIEnvironment();

	driver->setTextureCreationFlag(irr::video::ETCF_ALWAYS_32_BIT, true);

	//set other font
	env->getSkin()->setFont(env->getFont("../../media/fontlucida.png"));

	// add some help text
	env->addStaticText(
		L"Info",
		irr::core::rect<irr::s32>(10,711,250,765), true, true, 0, -1, true);

	// add camera
	irr::scene::ICameraSceneNode* camera =
		smgr->addCameraSceneNodeFPS(0,100.0f,1.2f);

	camera->setPosition(irr::core::vector3df(2700*2,255*2,2600*2));
	camera->setTarget(irr::core::vector3df(2397*2,343*2,2700*2));
	camera->setFarValue(42000.0f);

	// disable mouse cursor
	device->getCursorControl()->setVisible(false);
	
	CircleEngine::SequenceSelectorPtr allSeqSelector(new CircleEngine::SequenceSelector); 
	CircleEngine::PairNearSelectorPtr allCrossNearSelector(new CircleEngine::PairNearSelector(20)); 
	CircleEngine::CrossSelectorPtr allCrossSelector(new CircleEngine::CrossSelector()); 
	CircleEngine::SomeToOtherSelectorPtr gravOneToOtherSelector(new CircleEngine::SomeToOtherSelector); 
	CircleEngine::SomeToOtherSelectorPtr fricOneToOtherSelector(new CircleEngine::SomeToOtherSelector); 
	CircleEngine::PairBarSelectorPtr pairBarSelector(new CircleEngine::PairBarSelector);

	OBJ_Data obj_data;
	if (LoadObjFile(&obj_data, "../../FullCar.obj"))
	{
		CircleEngine::Point center = CircleEngine::Point(0,-175,0);
		CircleEngine::Point scale = CircleEngine::Point(30,30,30);
		
		for (size_t i = 0; i < obj_data.m_Points.size(); i++)
		{
			CircleCoordinator::ObjectPtr objContainer = CreateNewObject();
			
			CircleEngine::Point& cur_point = obj_data.m_Points[i];
			objContainer->Color = CircleCoordinator::ObjectColor(0, 1.0, 0, 1.0);
			objContainer->Detal = 6;
			CircleEngine::CircleObjectPtr& obj = objContainer->Obj;

			obj->Center = center + CircleEngine::Point(cur_point.x * scale.x, cur_point.y * scale.y, cur_point.z * scale.z);
			obj->Velocity = CircleEngine::Point(0,0,0);
			obj->Radius = 2.5;
			obj->Weight = obj->Radius * obj->Radius * obj->Radius / 8;
			
			circle_coordinator.AddObject(objContainer, driver, smgr);		
		}
		
		std::vector<CircleCoordinator::ObjectPtr> objectConts = circle_coordinator.GetObjects();
		
		for (size_t i = 0; i < obj_data.m_Edges.size(); i++)
		{
			size_t index1 = obj_data.m_Edges[i].first;
			size_t index2 = obj_data.m_Edges[i].second;
			
			if (index1 >= objectConts.size() || index2 >= objectConts.size())
				continue;

			CircleEngine::CircleObjectPtr obj1 = objectConts[index1]->Obj;
			CircleEngine::CircleObjectPtr obj2 = objectConts[index2]->Obj;
			CircleEngine::BarProperties prop;
			prop.Distance = obj1->Center.Distance(obj2->Center);

			pairBarSelector->Add(obj1, obj2, prop);
		}
	}

	#define rand_pmmax(maxValue) ((maxValue) * rand() / (RAND_MAX * 1.0) - (maxValue) / 2.0)
	#define rand_pmax(maxValue) ((maxValue) * rand() / (RAND_MAX * 1.0))

	const CircleEngine::CoordinateType grav_obj_radius = 115.0;
	const CircleEngine::CoordinateType fric_obj_radius = grav_obj_radius + 1.5;

	CircleCoordinator::ObjectPtr objGrav = CreateNewObject();

	objGrav->Color = CircleCoordinator::ObjectColor(0, 0.7, 0.2, 1.0);
	objGrav->Detal = 60;
	objGrav->Obj->Radius = grav_obj_radius;
	objGrav->Obj->Weight = 5000;

	circle_coordinator.AddObject(objGrav, driver, smgr);

	CircleCoordinator::ObjectPtr objFric = CreateNewObject();

	objFric->Color = CircleCoordinator::ObjectColor(0, 0, 0, 0.2);
	objFric->Detal = 60;
	objFric->Obj->Radius = fric_obj_radius;
	objFric->Obj->Weight = 0.0;

	circle_coordinator.AddObject(objFric, driver, smgr);

	CircleEngine::CircleObjectPtr obj1 = objGrav->Obj;
	CircleEngine::CircleObjectPtr obj2 = objFric->Obj;
	CircleEngine::BarProperties prop;
	prop.Distance = obj1->Center.Distance(obj2->Center);

	pairBarSelector->Add(obj1, obj2, prop);

	for (size_t i = 0; i < 500; i++)
	{
		CircleCoordinator::ObjectPtr objContainer = CreateNewObject();
		
		const CircleEngine::CoordinateType maxValue = 80.0;
		const CircleEngine::CoordinateType maxVelValue = 30.0;
		
		objContainer->Color = CircleCoordinator::ObjectColor(0.6, 0, 0, 1.0);			
		objContainer->Detal = 4;
		CircleEngine::CircleObjectPtr& obj = objContainer->Obj;

		obj->Center = CircleEngine::Point(rand_pmmax(maxValue), rand_pmmax(maxValue), rand_pmmax(maxValue));  // 
		obj->Velocity = CircleEngine::Point(rand_pmmax(maxVelValue), rand_pmmax(maxVelValue), 0); // CircleEngine::Point(0,0,0);
		obj->Radius = 1.8 + rand_pmmax(0.2);
		obj->Weight = obj->Radius * obj->Radius * obj->Radius / 8;		
		
		circle_coordinator.AddObject(objContainer, driver, smgr);
	}

	std::vector<CircleCoordinator::ObjectPtr> objectConts = circle_coordinator.GetObjects();
	for (size_t i = 0; i < objectConts.size(); i++)
	{
		CircleEngine::CircleObjectPtr obj = objectConts[i]->Obj;
		
		if (obj->Radius != fric_obj_radius)
		{
			allSeqSelector->Add(obj);
			allCrossNearSelector->Add(obj);
			allCrossSelector->Add(obj);
		}

		if (obj->Radius == grav_obj_radius)
			gravOneToOtherSelector->AddSome(obj);
		else
			gravOneToOtherSelector->AddOther(obj);

		if (obj->Radius == fric_obj_radius)
			fricOneToOtherSelector->AddSome(obj);
		else
			fricOneToOtherSelector->AddOther(obj);
	}

	CircleEngine::CoordinateType accuracy = 0.005;
	CircleEngine::RulePtr moveRule(new CircleEngine::RuleMove(allSeqSelector, accuracy));
	CircleEngine::RulePtr contactRule(new CircleEngine::RuleContact(allCrossNearSelector));
	CircleEngine::RulePtr gravityRule(new CircleEngine::RuleGravity(gravOneToOtherSelector, 15000, accuracy));
	CircleEngine::RulePtr fricRule(new CircleEngine::RuleFriction(fricOneToOtherSelector, 0.1));
	CircleEngine::RulePtr strongBarRule(new CircleEngine::RuleStrongBar(pairBarSelector));
	CircleEngine::RulePtr strongDistanceRule(new CircleEngine::RuleStrongDistance(pairBarSelector));
	
	circle_coordinator.AddRule(strongBarRule);
	circle_coordinator.AddRule(moveRule);
	circle_coordinator.AddRule(strongBarRule);
	circle_coordinator.AddRule(gravityRule);
	circle_coordinator.AddRule(fricRule);
	circle_coordinator.AddRule(contactRule);
	for (size_t i = 0; i < 100; ++i)
		circle_coordinator.AddRule(strongBarRule);
		
	g_WorkTime.Start();
	
	std::thread phys_th(PhysicsThread, std::ref(circle_coordinator));

	driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);
	irr::scene::ISceneNode* skydome = smgr->addSkyDomeSceneNode(driver->getTexture("../../media/Tycho_catalog_skymap_v2.0_(threshold_magnitude_3.0,_low-res).png"),16,8,0.95f,2.0f);
	driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, true);

	SystemEventReceiver receiver(skydome, circle_coordinator);
	device->setEventReceiver(&receiver);


	irr::video::SMaterial newMaterial;
	newMaterial.setTexture(0, driver->getTexture("../../media/wall.jpg"));
	irr::video::SMaterial newMaterial1;
	newMaterial1.setTexture(0, driver->getTexture("../../media/wall1.jpg"));
	
	irr::core::vector3df point = irr::core::vector3df(0, 0, 0);
	irr::core::vector3df point1 = irr::core::vector3df(1000, 0, 0);

	irr::scene::SMeshBuffer* buffer = CreateMeshScuare(point, newMaterial);
	irr::scene::SMeshBuffer* buffer1 = CreateMeshScuare(point1, newMaterial1);

	irr::scene::SMesh* mesh = new irr::scene::SMesh();
	mesh->addMeshBuffer(buffer);
	mesh->addMeshBuffer(buffer1);
	mesh->recalculateBoundingBox();

	irr::scene::ISceneNode* new_node = smgr->addMeshSceneNode(mesh);
    if (new_node)
    {
		new_node->setPosition(irr::core::vector3df(0,0,0));
		new_node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    }	
	
	size_t size = 32;
	CircleVectorZ object;
	object.resize(size);
	for (size_t z = 0; z < object.size(); ++z)
	{
		CircleVectorY& y_items = object[z];
		y_items.resize(size);
		for (size_t y = 0; y < y_items.size(); ++y)
		{
			CircleVectorX& x_items = y_items[y];
			x_items.resize(size);
			for (size_t x = 0; x < x_items.size(); ++x)
			{
				CircleItem& item = x_items[x];
				item.m_Color = irr::video::SColor(x * 255 / size, y * 255 / size, z * 255 / size, 255);
				irr::core::vector3df cur_vector(x + 0.5, y + 0.5, z + 0.5);
				irr::core::vector3df max_vector(x_items.size(), y_items.size(), object.size());
				irr::core::vector3df center_vector = cur_vector - max_vector * 0.5;
				if (center_vector.getLengthSQ() < 0.25 * size * size)
					item.m_Type = CircleItem::tpSolid;
			}
		}		
	}
	
	irr::scene::SMesh* mesh1 = CreateMeshFromObjectData(object, 50.0);
	irr::scene::ISceneNode* object_node = smgr->addMeshSceneNode(mesh1);
	if (object_node)
	{
		object_node->setPosition(irr::core::vector3df(0,0,0));
		object_node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
		object_node->setMaterialFlag(irr::video::EMF_GOURAUD_SHADING, false);
		object_node->setMaterialFlag(irr::video::EMF_BILINEAR_FILTER, false);	
		object_node->setMaterialFlag(irr::video::EMF_ANTI_ALIASING, false);
	}
	
	/*irr::scene::ISceneNode* sphere_node = smgr->addSphereSceneNode(500, 200);
	if (sphere_node)
	{
		sphere_node->setPosition(irr::core::vector3df(0,0,0));
		sphere_node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	}*/
	
	OnDisplay(circle_coordinator);

	int lastFPS = -1;
	while(device->run())
	if (device->isWindowActive())
	{
		driver->beginScene(true, true, 0 );

		smgr->drawAll();
		env->drawAll();

		driver->endScene();

		// display frames per second in window title
		int fps = driver->getFPS();
		if (lastFPS != fps)
		{
			irr::core::stringw str = L"Terrain Renderer - Irrlicht Engine [";
			str += driver->getName();
			str += "] FPS:";
			str += fps;
			// Also print terrain height of current camera position
			// We can use camera position because terrain is located at coordinate origin
			str += " Height: ";

			device->setWindowCaption(str.c_str());
			lastFPS = fps;
		}
		//OnDisplay(circle_coordinator);
	}

	device->drop();
	
	std::cout << s_AddFaceCount << " ";
	g_NeedExit = true;
	phys_th.join();
	
	return 0;
}
