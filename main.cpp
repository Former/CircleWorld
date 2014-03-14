#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <string>
#include <sstream>

#include <thread>
#include <mutex>

#include "CircleCoordinator.h"
#include "GetWorkTime.h"
#include "Import_Obj.h"

void OnResize(int width,int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (height)
		gluPerspective(30.0f, width / height, 0.1f, 1500.0f);
	gluLookAt(0,-800,20, 0,0,0, 0,1,0);
	glMatrixMode(GL_MODELVIEW);
}    

void OnExit();

CircleEngine::CircleCoordinator g_CircleCoordinator;
CircleEngine::PairBarSelectorPtr g_LinesSelector;
std::mutex g_GuiMutex;
size_t s_DrawCount = 0;
GetWorkTime	g_WorkTime;
size_t s_PhysCount = 0;
std::thread* g_PhysThreag;
bool g_NeedExit = false;

struct SExiter
{
	~SExiter()
	{
		OnExit();
	}
} g_Exiter;

void OnExit()
{
	g_NeedExit = true;
	g_PhysThreag->join();
}

void OnKeyboard(unsigned char key, int x, int y)
{
	std::vector<std::pair<size_t, size_t> > well_index;
	well_index.push_back(std::pair<size_t, size_t>(5, 6));
	well_index.push_back(std::pair<size_t, size_t>(57, 58));
	well_index.push_back(std::pair<size_t, size_t>(116, 118));
	well_index.push_back(std::pair<size_t, size_t>(145, 147));
	static int index = 145; // 5, 57, 116(118), 145(147)
	static double dir = 1.0;

	switch (key)
	{
	case 'a':
		glRotated(-1, 0, 0, 1); 			
		break;
	case 'd':
		glRotated(1, 0, 0, 1); 			
		break;
	case 'w':
		glRotated(-1, 1, 0, 0); 			
		break;
	case 's':
		glRotated(1, 1, 0, 0);
		break;
	case 'r':
		{
			for (size_t i = 0; i < well_index.size(); ++i)
			{
				const std::pair<size_t, size_t>& cur_pair = well_index[i];
				
				std::vector<CircleEngine::CircleCoordinator::ObjectPtr> objectConts = g_CircleCoordinator.GetObjects();
				CircleEngine::Point difCenter = (objectConts[cur_pair.first]->Obj->Center - objectConts[cur_pair.second]->Obj->Center);
				difCenter = difCenter / difCenter.Distance(CircleEngine::Point(0,0,0));
				objectConts[cur_pair.first]->Obj->Velocity =  difCenter * 2500.0 * dir;
			}
		}
		break;
	case 't':
		{
			std::vector<CircleEngine::CircleCoordinator::ObjectPtr> objectConts = g_CircleCoordinator.GetObjects();
			objectConts[index]->Obj->Velocity = (objectConts[index]->Obj->Center - objectConts[index + 1]->Obj->Center) * 150.0 * dir;
		}
		break;
	case 'i':
		index++;
		break;
	case 'o':
		dir *= -1.0;
		break;
	}
}

void DebugOtput(const std::string& string)
{
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
	glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
	glRasterPos2i(-10, -200);
	glColor3d(1.0, 1.0, 1.0);
	for(size_t i =0; i < string.size(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}  
	glEnable(GL_LIGHTING);
	glPopAttrib();
}

void OnDisplay(void)
{
	std::vector<CircleEngine::CircleCoordinator::ObjectPtr> objects;
	{
		std::lock_guard<std::mutex> lock(g_GuiMutex);

		objects = g_CircleCoordinator.GetObjects();
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (size_t i = 0; i < objects.size(); i++)
	{
		glPushMatrix();
		
		CircleEngine::CircleCoordinator::ObjectColor color = objects[i]->Color;
		CircleEngine::CircleObjectPtr obj = objects[i]->Obj;
		size_t detal = objects[i]->Detal;

		glTranslated(obj->Center.x, obj->Center.y, obj->Center.z);
		glColor4d(color.red, color.green, color.blue, color.alpha);
		glutSolidSphere(obj->Radius, detal, detal);
		
		glPopMatrix();
	}
	
	// Draw lines	
	glLineWidth(2); // ширина линии 
	glBegin(GL_LINES);
	for (CircleEngine::PairBarSelector::IteratorPtr it = g_LinesSelector->Begin(); !it->IsEnd(); it->Next())
	{			
		const CircleEngine::CircleObjectPtr& obj1 = it->GetFirst();
		const CircleEngine::CircleObjectPtr& obj2 = it->GetSecond();
		
		glColor3d(1, 1, 1);
		glVertex3d(obj1->Center.x, obj1->Center.y, obj1->Center.z);
		glVertex3d(obj2->Center.x, obj2->Center.y, obj2->Center.z);
	}
	glEnd();
	
	// Draw FPS
	s_DrawCount++;
	std::stringstream sstr;
	sstr << "fps " << s_DrawCount / g_WorkTime.GetCurrentTime() << " ph/ps " << s_PhysCount / g_WorkTime.GetCurrentTime();
	DebugOtput(sstr.str());
	
	if (s_DrawCount > 100)
	{
		g_WorkTime.Start();
		s_DrawCount = 0;
		s_PhysCount = 0;
	}	

	glutSwapBuffers();
}

void PhysicsThread()
{
	try
	{
		while (true)
		{
			std::chrono::milliseconds sleep_delay(1);
			std::this_thread::sleep_for(sleep_delay);
			std::lock_guard<std::mutex> lock(g_GuiMutex);
			
			if (g_NeedExit)
				return;
			
			g_CircleCoordinator.DoStep();
			s_PhysCount++;
		}
	}
	catch(...)
	{
		return;
	}
}

CircleEngine::CircleCoordinator::ObjectPtr CreateNewObject()
{
	CircleEngine::CircleCoordinator::ObjectPtr objContainer(new CircleEngine::CircleCoordinator::Object);
	CircleEngine::CircleObjectPtr obj(new CircleEngine::CircleObject);
	objContainer->Obj = obj;
	
	return objContainer;
}

int main1(int argc, char** argv)
{
	float pos[4] = {0,0,300,1};
	float dir[3] = {-1,-1,-1};
	GLfloat mat_specular[] = {1,1,1,1};

	glutInit(&argc, argv);

	glutInitWindowPosition(50, 10);
	glutInitWindowSize(800, 600);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_ACCUM);
	glutCreateWindow("Circle World");
	
	glutIdleFunc(OnDisplay);
	glutDisplayFunc(OnDisplay);
	glutReshapeFunc(OnResize);
	glutKeyboardFunc(OnKeyboard);
  
	glShadeModel(GL_SMOOTH);
	
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, 128.0);

	CircleEngine::SequenceSelectorPtr allSeqSelector(new CircleEngine::SequenceSelector); 
	CircleEngine::PairNearSelectorPtr allCrossNearSelector(new CircleEngine::PairNearSelector(20)); 
	CircleEngine::CrossSelectorPtr allCrossSelector(new CircleEngine::CrossSelector()); 
	CircleEngine::SomeToOtherSelectorPtr gravOneToOtherSelector(new CircleEngine::SomeToOtherSelector); 
	CircleEngine::SomeToOtherSelectorPtr fricOneToOtherSelector(new CircleEngine::SomeToOtherSelector); 
	CircleEngine::PairBarSelectorPtr pairBarSelector(new CircleEngine::PairBarSelector);
	g_LinesSelector = pairBarSelector;

	OBJ_Data obj_data;
	if (LoadObjFile(&obj_data, "../../FullCar.obj"))
	{
		CircleEngine::Point center = CircleEngine::Point(0,-175,0);
		CircleEngine::Point scale = CircleEngine::Point(30,30,30);
		
		for (size_t i = 0; i < obj_data.m_Points.size(); i++)
		{
			CircleEngine::CircleCoordinator::ObjectPtr objContainer = CreateNewObject();
			
			CircleEngine::Point& cur_point = obj_data.m_Points[i];
			objContainer->Color = CircleEngine::CircleCoordinator::ObjectColor(0, 1.0, 0, 1.0);
			objContainer->Detal = 6;
			CircleEngine::CircleObjectPtr& obj = objContainer->Obj;

			obj->Center = center + CircleEngine::Point(cur_point.x * scale.x, cur_point.y * scale.y, cur_point.z * scale.z);
			obj->Velocity = CircleEngine::Point(0,0,0);
			obj->Radius = 2.5;
			obj->Weight = obj->Radius * obj->Radius * obj->Radius / 8;
			
			g_CircleCoordinator.AddObject(objContainer);		
		}
		
		std::vector<CircleEngine::CircleCoordinator::ObjectPtr> objectConts = g_CircleCoordinator.GetObjects();
		
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

	CircleEngine::CircleCoordinator::ObjectPtr objGrav = CreateNewObject();

	objGrav->Color = CircleEngine::CircleCoordinator::ObjectColor(0, 0.7, 0.2, 1.0);
	objGrav->Detal = 60;
	objGrav->Obj->Radius = grav_obj_radius;
	objGrav->Obj->Weight = 5000;

	g_CircleCoordinator.AddObject(objGrav);

	CircleEngine::CircleCoordinator::ObjectPtr objFric = CreateNewObject();

	objFric->Color = CircleEngine::CircleCoordinator::ObjectColor(0, 0, 0, 0.2);
	objFric->Detal = 60;
	objFric->Obj->Radius = fric_obj_radius;
	objFric->Obj->Weight = 0.0;

	g_CircleCoordinator.AddObject(objFric);

	CircleEngine::CircleObjectPtr obj1 = objGrav->Obj;
	CircleEngine::CircleObjectPtr obj2 = objFric->Obj;
	CircleEngine::BarProperties prop;
	prop.Distance = obj1->Center.Distance(obj2->Center);

	pairBarSelector->Add(obj1, obj2, prop);

	for (size_t i = 0; i < 7000; i++)
	{
		CircleEngine::CircleCoordinator::ObjectPtr objContainer = CreateNewObject();
		
		const CircleEngine::CoordinateType maxValue = 80.0;
		const CircleEngine::CoordinateType maxVelValue = 30.0;
		
		objContainer->Color = CircleEngine::CircleCoordinator::ObjectColor(0.6, 0, 0, 1.0);			
		objContainer->Detal = 4;
		CircleEngine::CircleObjectPtr& obj = objContainer->Obj;

		obj->Center = CircleEngine::Point(rand_pmmax(maxValue), rand_pmmax(maxValue), rand_pmmax(maxValue));  // 
		obj->Velocity = CircleEngine::Point(rand_pmmax(maxVelValue), rand_pmmax(maxVelValue), 0); // CircleEngine::Point(0,0,0);
		obj->Radius = 1.8 + rand_pmmax(0.2);
		obj->Weight = obj->Radius * obj->Radius * obj->Radius / 8;		
		
		g_CircleCoordinator.AddObject(objContainer);
	}

	std::vector<CircleEngine::CircleCoordinator::ObjectPtr> objectConts = g_CircleCoordinator.GetObjects();
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
	
	g_CircleCoordinator.AddRule(strongBarRule);
	g_CircleCoordinator.AddRule(moveRule);
	g_CircleCoordinator.AddRule(strongBarRule);
	g_CircleCoordinator.AddRule(gravityRule);
	g_CircleCoordinator.AddRule(fricRule);
	g_CircleCoordinator.AddRule(contactRule);
	for (size_t i = 0; i < 100; ++i)
		g_CircleCoordinator.AddRule(strongBarRule);
		
	g_WorkTime.Start();
	
	std::thread phys_th(PhysicsThread);
	g_PhysThreag = &phys_th;
	
    glutMainLoop();
	OnExit();
	
	return 0;
}


#include <irrlicht.h>
#include "driverChoice.h"

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif


class MyEventReceiver : public IEventReceiver
{
public:

	MyEventReceiver(scene::ISceneNode* terrain, scene::ISceneNode* skybox, scene::ISceneNode* skydome) :
		Terrain(terrain), Skybox(skybox), Skydome(skydome), showBox(true), showDebug(false)
	{
		Skybox->setVisible(showBox);
		Skydome->setVisible(!showBox);
	}

	bool OnEvent(const SEvent& event)
	{
		// check if user presses the key 'W' or 'D'
		if (event.EventType == irr::EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown)
		{
			switch (event.KeyInput.Key)
			{
			case irr::KEY_KEY_W: // switch wire frame mode
				Terrain->setMaterialFlag(video::EMF_WIREFRAME,
						!Terrain->getMaterial(0).Wireframe);
				Terrain->setMaterialFlag(video::EMF_POINTCLOUD, false);
				return true;
			case irr::KEY_KEY_P: // switch wire frame mode
				Terrain->setMaterialFlag(video::EMF_POINTCLOUD,
						!Terrain->getMaterial(0).PointCloud);
				Terrain->setMaterialFlag(video::EMF_WIREFRAME, false);
				return true;
			case irr::KEY_KEY_D: // toggle detail map
				Terrain->setMaterialType(
					Terrain->getMaterial(0).MaterialType == video::EMT_SOLID ?
					video::EMT_DETAIL_MAP : video::EMT_SOLID);
				return true;
			case irr::KEY_KEY_S: // toggle skies
				showBox=!showBox;
				Skybox->setVisible(showBox);
				Skydome->setVisible(!showBox);
				return true;
			case irr::KEY_KEY_X: // toggle debug information
				showDebug=!showDebug;
				Terrain->setDebugDataVisible(showDebug?scene::EDS_BBOX_ALL:scene::EDS_OFF);
				return true;
			default:
				break;
			}
		}

		return false;
	}

private:
	scene::ISceneNode* Terrain;
	scene::ISceneNode* Skybox;
	scene::ISceneNode* Skydome;
	bool showBox;
	bool showDebug;
};


/*
The start of the main function starts like in most other example. We ask the
user for the desired renderer and start it up. This time with the advanced
parameter handling.
*/
int main()
{
	// ask user for driver
	video::E_DRIVER_TYPE driverType=video::EDT_OPENGL; //driverChoiceConsole();
	if (driverType==video::EDT_COUNT)
		return 1;

	// create device with full flexibility over creation parameters
	// you can add more parameters if desired, check irr::SIrrlichtCreationParameters
	irr::SIrrlichtCreationParameters params;
	params.DriverType=driverType;
	params.WindowSize=core::dimension2d<u32>(640, 480);
	IrrlichtDevice* device = createDeviceEx(params);

	if (device == 0)
		return 1; // could not create selected driver.

	
	/*
	First, we add standard stuff to the scene: A nice irrlicht engine
	logo, a small help text, a user controlled camera, and we disable
	the mouse cursor.
	*/

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* env = device->getGUIEnvironment();

	driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);

	// add irrlicht logo
	env->addImage(driver->getTexture("../../media/irrlichtlogo2.png"),
		core::position2d<s32>(10,10));

	//set other font
	env->getSkin()->setFont(env->getFont("../../media/fontlucida.png"));

	// add some help text
	env->addStaticText(
		L"Press 'W' to change wireframe mode\nPress 'D' to toggle detail map\nPress 'S' to toggle skybox/skydome",
		core::rect<s32>(10,421,250,475), true, true, 0, -1, true);

	// add camera
	scene::ICameraSceneNode* camera =
		smgr->addCameraSceneNodeFPS(0,100.0f,1.2f);

	camera->setPosition(core::vector3df(2700*2,255*2,2600*2));
	camera->setTarget(core::vector3df(2397*2,343*2,2700*2));
	camera->setFarValue(42000.0f);

	// disable mouse cursor
	device->getCursorControl()->setVisible(false);

	/*
	Here comes the terrain renderer scene node: We add it just like any
	other scene node to the scene using
	ISceneManager::addTerrainSceneNode(). The only parameter we use is a
	file name to the heightmap we use. A heightmap is simply a gray scale
	texture. The terrain renderer loads it and creates the 3D terrain from
	it.

	To make the terrain look more big, we change the scale factor of
	it to (40, 4.4, 40). Because we don't have any dynamic lights in the
	scene, we switch off the lighting, and we set the file
	terrain-texture.jpg as texture for the terrain and detailmap3.jpg as
	second texture, called detail map. At last, we set the scale values for
	the texture: The first texture will be repeated only one time over the
	whole terrain, and the second one (detail map) 20 times.
	*/

	// add terrain scene node
	scene::ITerrainSceneNode* terrain = smgr->addTerrainSceneNode(
		"../../media/terrain-heightmap.bmp",
		0,					// parent node
		-1,					// node id
		core::vector3df(0.f, 0.f, 0.f),		// position
		core::vector3df(0.f, 0.f, 0.f),		// rotation
		core::vector3df(40.f, 4.4f, 40.f),	// scale
		video::SColor ( 255, 255, 255, 255 ),	// vertexColor
		5,					// maxLOD
		scene::ETPS_17,				// patchSize
		4					// smoothFactor
		);

	terrain->setMaterialFlag(video::EMF_LIGHTING, false);

	terrain->setMaterialTexture(0,
			driver->getTexture("../../media/terrain-texture.jpg"));
	terrain->setMaterialTexture(1,
			driver->getTexture("../../media/detailmap3.jpg"));
	
	terrain->setMaterialType(video::EMT_DETAIL_MAP);

	terrain->scaleTexture(1.0f, 20.0f);

	/*
	To be able to do collision with the terrain, we create a triangle selector.
	If you want to know what triangle selectors do, just take a look into the
	collision tutorial. The terrain triangle selector works together with the
	terrain. To demonstrate this, we create a collision response animator
	and attach it to the camera, so that the camera will not be able to fly
	through the terrain.
	*/

	// create triangle selector for the terrain	
	scene::ITriangleSelector* selector
		= smgr->createTerrainTriangleSelector(terrain, 0);
	terrain->setTriangleSelector(selector);

	// create collision response animator and attach it to the camera
	scene::ISceneNodeAnimator* anim = smgr->createCollisionResponseAnimator(
		selector, camera, core::vector3df(60,100,60),
		core::vector3df(0,0,0),
		core::vector3df(0,50,0));
	selector->drop();
	camera->addAnimator(anim);
	anim->drop();

	/* If you need access to the terrain data you can also do this directly via the following code fragment.
	*/
	scene::CDynamicMeshBuffer* buffer = new scene::CDynamicMeshBuffer(video::EVT_2TCOORDS, video::EIT_16BIT);
	terrain->getMeshBufferForLOD(*buffer, 0);
	video::S3DVertex2TCoords* data = (video::S3DVertex2TCoords*)buffer->getVertexBuffer().getData();
	// Work on data or get the IndexBuffer with a similar call.
	buffer->drop(); // When done drop the buffer again.

	/*
	To make the user be able to switch between normal and wireframe mode,
	we create an instance of the event receiver from above and let Irrlicht
	know about it. In addition, we add the skybox which we already used in
	lots of Irrlicht examples and a skydome, which is shown mutually
	exclusive with the skybox by pressing 'S'.
	*/

	// create skybox and skydome
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);

	scene::ISceneNode* skybox=smgr->addSkyBoxSceneNode(
		driver->getTexture("../../media/irrlicht2_up.jpg"),
		driver->getTexture("../../media/irrlicht2_dn.jpg"),
		driver->getTexture("../../media/irrlicht2_lf.jpg"),
		driver->getTexture("../../media/irrlicht2_rt.jpg"),
		driver->getTexture("../../media/irrlicht2_ft.jpg"),
		driver->getTexture("../../media/irrlicht2_bk.jpg"));
	scene::ISceneNode* skydome=smgr->addSkyDomeSceneNode(driver->getTexture("../../media/skydome.jpg"),16,8,0.95f,2.0f);

	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);

	// create event receiver
	MyEventReceiver receiver(terrain, skybox, skydome);
	device->setEventReceiver(&receiver);

	/*
	That's it, draw everything.
	*/

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
			core::stringw str = L"Terrain Renderer - Irrlicht Engine [";
			str += driver->getName();
			str += "] FPS:";
			str += fps;
			// Also print terrain height of current camera position
			// We can use camera position because terrain is located at coordinate origin
			str += " Height: ";
			str += terrain->getHeight(camera->getAbsolutePosition().X,
					camera->getAbsolutePosition().Z);

			device->setWindowCaption(str.c_str());
			lastFPS = fps;
		}
	}

	device->drop();
	
	return 0;
}
