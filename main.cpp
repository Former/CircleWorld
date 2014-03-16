#include "Common.h"

#include "CircleCoordinator.h"
#include "CreateMeshFromObjectData.h"
#include "GetWorkTime.h"
#include "Import_Obj.h"
#include "LOD_Object.h"

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

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

extern int s_AddFaceCount;

static irr::scene::ISceneNode* MakeNodeFromMesh(irr::scene::SMesh* a_Mesh, irr::scene::ISceneManager* a_SMgr)
{
	irr::scene::ISceneNode* object_node = a_SMgr->addMeshSceneNode(a_Mesh);
	if (object_node)
	{
		object_node->setDebugDataVisible(irr::scene::EDS_BBOX);
		object_node->setPosition(irr::core::vector3df(0,0,0));
		object_node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	}
	
	return object_node;
}

static void FillItems(IN OUT CircleVectorZ& a_Object, const size_t& a_Size, const irr::core::vector3df& a_Center, const double& a_Radius, const CircleItem::Type& a_Type)
{
	size_t size = a_Size;
	a_Object.resize(size);
	for (size_t z = 0; z < a_Object.size(); ++z)
	{
		CircleVectorY& y_items = a_Object[z];
		y_items.resize(size);
		for (size_t y = 0; y < y_items.size(); ++y)
		{
			CircleVectorX& x_items = y_items[y];
			x_items.resize(size);
			for (size_t x = 0; x < x_items.size(); ++x)
			{
				CircleItem& item = x_items[x];
				irr::core::vector3df cur_vector(x + 0.5, y + 0.5, z + 0.5);
				irr::core::vector3df max_vector(x_items.size(), y_items.size(), a_Object.size());
				irr::core::vector3df vector = cur_vector - a_Center;
				if (vector.getLengthSQ() < a_Radius * a_Radius)
					item.m_Type = a_Type;
			}
		}		
	}
}

class CObjectCreationStrategy : public ObjectCreationStrategy
{
public:
	CObjectCreationStrategy(irr::video::IVideoDriver* a_Driver, const size_t& a_Size)
	{
		m_Driver = a_Driver;
		m_Size = a_Size;
		m_Material.setTexture(0, m_Driver->getTexture("../../media/wall.jpg"));
	}
	
	virtual irr::video::SColor GetColor(const CircleItem& a_Item, const Point& a_Point) override
	{
		return irr::video::SColor(a_Point.x * 255 / m_Size, a_Point.x * 255 / m_Size, a_Point.x * 255 / m_Size, a_Point.x * 255 / m_Size);
	}
	
	virtual irr::video::SMaterial GetMaterial(const CircleItem& a_Item) override
	{
		return m_Material;
	}
	
private:
	irr::video::IVideoDriver* m_Driver;
	size_t m_Size;
	irr::video::SMaterial m_Material;
};

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
	camera->setFarValue(200000.0f);	

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

	size_t size = 256;
	CircleVectorZ object;
	irr::core::vector3df max_vector(size, size, size);
	FillItems(object, size, max_vector * 0.5, size * 0.5, CircleItem::tpSolid);

	FillItems(object, size, max_vector * 0.25, size * 0.15, CircleItem::tpNone);
	FillItems(object, size, max_vector * 0.75, size * 0.15, CircleItem::tpNone);
	
	struct LOD_Settings
	{
		double m_Distance;
		size_t m_DrawStep;
	};
	
	const LOD_Settings settings[] = 
	{
		{10000, 1},
		{40000, 4},
		{160000, 16},
	};
	
	size_t div_step = 64;
	ObjectCreationStrategyPtr strategy(new CObjectCreationStrategy(driver, size));
	
	std::vector<LOD_Object_Vector> lod_objects_vectors;
	for (size_t i = 0; i < ARRAY_SIZE(settings); ++i)
	{
		const LOD_Settings& item = settings[i];
		SMeshVector meshs = CreateMeshFromObjectData(object, strategy, 50.0, item.m_DrawStep, div_step);
	
		lod_objects_vectors.resize(meshs.size());

		for (size_t j = 0; j < meshs.size(); ++j)
		{
			irr::scene::SMesh* mesh = meshs[j];
			irr::scene::ISceneNode* object_node = MakeNodeFromMesh(mesh, smgr);
			
			LOD_Object_Item new_item(object_node, item.m_Distance);
			lod_objects_vectors[j].push_back(new_item);
		}
	}
	
	object.clear();
	
	std::vector<LOD_ObjectPtr> lod_objects;
	for (size_t i = 0; i < lod_objects_vectors.size(); ++i)
	{
		LOD_ObjectPtr lod_item(new LOD_Object(lod_objects_vectors[i]));
		
		lod_objects.push_back(lod_item);
	}
	
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

			device->setWindowCaption(str.c_str());
			lastFPS = fps;
		}
		//OnDisplay(circle_coordinator);

		for (size_t i = 0; i < lod_objects.size(); ++i)
			lod_objects[i]->SetVisibleOneItem(camera->getAbsolutePosition());
	}

	device->drop();
	
	std::cout << s_AddFaceCount << " ";
	g_NeedExit = true;
	phys_th.join();
	
	return 0;
}
