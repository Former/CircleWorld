#include "Common.h"

#include "CircleCoordinator.h"
#include "CreateMeshFromObjectData.h"
#include "GetWorkTime.h"
#include "Import_Obj.h"
#include "LOD_Object.h"
#include "SolidObject.h"

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
		static double dir = 1.0;

		if (event.EventType == irr::EET_KEY_INPUT_EVENT)   //  && !event.KeyInput.PressedDown
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

class CObjectDrawStrategy : public ObjectDrawStrategy
{
public:
	CObjectDrawStrategy(const ObjectDataPtr& a_ObjectData, irr::video::IVideoDriver* a_Driver, const size_t& a_Size);

	virtual irr::video::SColor GetColor(const CircleItem& a_Item, const IntPoint& a_Point) override;
	
	virtual const CircleItem& GetItem(const IntPoint& a_CurPoint) override;

	virtual irr::video::SMaterial GetMaterial(const CircleItem& a_Item) override;

	virtual bool AddFace(const IntPoint& a_CurPoint, const IntPoint& a_NearPoint, const size_t& a_DrawStep) override;

	virtual bool IgnoreFace(const IntPoint& a_CurPoint, const size_t& a_DrawStep) override;

	virtual size_t GetThreadCount() override;

	virtual double GetStep() override;

private:
	irr::video::IVideoDriver* m_Driver;
	size_t m_Size;
	irr::video::SMaterial m_SolidMaterial;
	irr::video::SMaterial m_WaterMaterial;
	ObjectDataPtr m_ObjectData;
};

CObjectDrawStrategy::CObjectDrawStrategy(const ObjectDataPtr& a_ObjectData, irr::video::IVideoDriver* a_Driver, const size_t& a_Size)
	: m_ObjectData(a_ObjectData)
{
	m_Driver = a_Driver;
	m_Size = a_Size;
	m_SolidMaterial.setTexture(0, m_Driver->getTexture("../../media/wall.jpg"));
	m_WaterMaterial.setTexture(0, m_Driver->getTexture("../../media/water.jpg"));
}

const CircleItem&  CObjectDrawStrategy::GetItem(const IntPoint& a_CurPoint)
{
	CircleVectorZ& object_data = *m_ObjectData;
	return object_data[a_CurPoint.z][a_CurPoint.y][a_CurPoint.x];
}

irr::video::SColor CObjectDrawStrategy::GetColor(const CircleItem& a_Item, const IntPoint& a_Point)
{
	return irr::video::SColor(a_Point.x * 255 / m_Size, a_Point.x * 255 / m_Size, a_Point.x * 255 / m_Size, a_Point.x * 255 / m_Size);
}

bool CObjectDrawStrategy::IgnoreFace(const IntPoint& a_CurPoint, const size_t& a_DrawStep)
{
	CircleVectorZ& object_data = *m_ObjectData;
	const size_t max_z = int(object_data.size());
	const size_t max_y = int(object_data[0].size());
	const size_t max_x = int(object_data[0][0].size());

	for (size_t z = a_CurPoint.z;  z < max_z && z < a_CurPoint.z + a_DrawStep; ++z)
	{
		for (size_t y = a_CurPoint.y;  y < max_y && y < a_CurPoint.y + a_DrawStep; ++y)
		{
			for (size_t x = a_CurPoint.x;  x < max_x && x < a_CurPoint.x + a_DrawStep; ++x)
			{
				const CircleItem& near_item = object_data[z][y][x];
				if (near_item.m_Type != CircleItem::tpNone)
					return false;
			}
		}
	}

	return true;
}

bool CObjectDrawStrategy::AddFace(const IntPoint& a_CurPoint, const IntPoint& a_NearPoint, const size_t& a_DrawStep)
{
	CircleVectorZ& object_data = *m_ObjectData;
	const size_t max_z = int(object_data.size());
	const size_t max_y = int(object_data[0].size());
	const size_t max_x = int(object_data[0][0].size());

	for (size_t z = a_NearPoint.z;  z < max_z && z < a_NearPoint.z + a_DrawStep; ++z)
	{
		for (size_t y = a_NearPoint.y;  y < max_y && y < a_NearPoint.y + a_DrawStep; ++y)
		{
			for (size_t x = a_NearPoint.x;  x < max_x && x < a_NearPoint.x + a_DrawStep; ++x)
			{
				const CircleItem& near_item = object_data[z][y][x];
				if (near_item.m_Type == CircleItem::tpNone)
					return true;
			}
		}
	}

	return false;
}

irr::video::SMaterial CObjectDrawStrategy::GetMaterial(const CircleItem& a_Item)
{
	switch (a_Item.m_Type)
	{
	case CircleItem::tpWater:
		return m_WaterMaterial;
	}

	return m_SolidMaterial;
}

size_t CObjectDrawStrategy::GetThreadCount()
{
	return sysconf(_SC_NPROCESSORS_ONLN);
}

double CObjectDrawStrategy::GetStep()
{
	return 50.0;
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

	size_t size = 1024 + 2;
	ObjectDataPtr object(new CircleVectorZ);
	irr::core::vector3df max_vector(size, size, size);
	FillItems(*object, size, max_vector * 0.5, size * 0.45, CircleItem::tpSolid);

	FillItems(*object, size, max_vector * 0.25, size * 0.15, CircleItem::tpNone);
	FillItems(*object, size, max_vector * 0.75, size * 0.15, CircleItem::tpNone);
	FillItems(*object, size, max_vector * 0.75, size * 0.10, CircleItem::tpWater);

	size_t div_step = 64;

	irr::core::vector3df center(0, 0, 10000);
	ObjectDrawStrategyPtr strategy(new CObjectDrawStrategy(object, driver, size));

	SolidObject solid_object(object, strategy, div_step, smgr);
	solid_object.SetPosition(center);

	irr::scene::ISceneNode* sphere_node = smgr->addSphereSceneNode(1000, 100);
	if (sphere_node)
	{
		sphere_node->setPosition(irr::core::vector3df(10000,0,0));
		sphere_node->setMaterialTexture(0, driver->getTexture("../../media/wall.jpg"));
		sphere_node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
	}

	OnDisplay(circle_coordinator);
	solid_object.Draw(camera->getAbsolutePosition());

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

			if ((*object)[0][0][0].m_Type)
				sphere_node->setRotation(sphere_node->getRotation() + irr::core::vector3df(0.2, 0.3, 0.1));

			solid_object.Draw(camera->getAbsolutePosition());
			//solid_object.DoStep();
			solid_object.SetRotation(solid_object.GetRotation() + irr::core::vector3df(0.0, 0.0, 0.05));
		}

	device->drop();

	std::cout << s_AddFaceCount << " ";
	g_NeedExit = true;
	phys_th.join();

	return 0;
}
