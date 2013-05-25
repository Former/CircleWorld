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
	gluLookAt(0,-250,20, 0,0,0, 0,1,0);
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
	static int index = 5;
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
			std::vector<CircleEngine::CircleCoordinator::ObjectPtr> objectConts = g_CircleCoordinator.GetObjects();
			objectConts[index]->Obj->Velocity = (objectConts[index]->Obj->Center - objectConts[index + 1]->Obj->Center) * 20.0;
		}
		break;
	case 'i':
		index++;
		break;
	}
}

void DebugOtput(const std::string& string)
{
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
	glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
	glRasterPos2i(-10, -100);
	glColor3d(1.0, 1.0, 1.0);
	for(int i =0; i < string.size(); i++)
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
		glColor3d(color.red, color.green, color.blue);
		glutSolidSphere(obj->Radius, detal, detal);
		
		glPopMatrix();
	}
	
	// Draw lines	
	glLineWidth(1); // ширина линии 
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

int main(int argc, char** argv)
{
	float pos[4] = {0,0,0,1};
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
	CircleEngine::SomeToOtherSelectorPtr oneToOtherSelector(new CircleEngine::SomeToOtherSelector); 
	CircleEngine::PairBarSelectorPtr pairBarSelector(new CircleEngine::PairBarSelector);
	g_LinesSelector = pairBarSelector;

	OBJ_Data obj_data;
	if (LoadObjFile(&obj_data, "../../FullCar.obj"))
	{
		CircleEngine::Point center = CircleEngine::Point(0,-55,0);
		CircleEngine::Point scale = CircleEngine::Point(15,15,15);
		
		for (size_t i = 0; i < obj_data.m_Points.size(); i++)
		{
			CircleEngine::CircleCoordinator::ObjectPtr objContainer(new CircleEngine::CircleCoordinator::Object);
			CircleEngine::CircleObjectPtr obj(new CircleEngine::CircleObject);
			objContainer->Obj = obj;
			
			CircleEngine::Point& cur_point = obj_data.m_Points[i];
			objContainer->Color = CircleEngine::CircleCoordinator::ObjectColor(0, 1.0, 0);
			objContainer->Detal = 6;
			obj->Center = center + CircleEngine::Point(cur_point.x * scale.x, cur_point.y * scale.y, cur_point.z * scale.z);
			obj->Velocity = CircleEngine::Point(0,0,0);
			obj->Radius = .5;
			obj->Weight = obj->Radius * obj->Radius * obj->Radius / 28;
			
			g_CircleCoordinator.AddObject(objContainer);		
		}
		
		std::vector<CircleEngine::CircleCoordinator::ObjectPtr> objectConts = g_CircleCoordinator.GetObjects();
		
		for (size_t i = 0; i < obj_data.m_Edges.size(); i++)
		{
			CircleEngine::BarProperties prop;
			
			size_t index1 = obj_data.m_Edges[i].first;
			size_t index2 = obj_data.m_Edges[i].second;
			
			if (index1 >= objectConts.size() || index2 >= objectConts.size())
				continue;

			CircleEngine::CircleObjectPtr obj1 = objectConts[index1]->Obj;
			CircleEngine::CircleObjectPtr obj2 = objectConts[index2]->Obj;
			prop.Distance = obj1->Center.Distance(obj2->Center);

			pairBarSelector->Add(obj1, obj2, prop);
		}
	}

	#define rand_pmmax(maxValue) ((maxValue) * rand() / (RAND_MAX * 1.0) - (maxValue) / 2.0)
	#define rand_pmax(maxValue) ((maxValue) * rand() / (RAND_MAX * 1.0))

	const CircleEngine::CoordinateType big_radius = 45.0;

	for (size_t i = 0; i < 1000; i++)
	{
		CircleEngine::CircleCoordinator::ObjectPtr objContainer(new CircleEngine::CircleCoordinator::Object);
		CircleEngine::CircleObjectPtr obj(new CircleEngine::CircleObject);
		objContainer->Obj = obj;
		
		const CircleEngine::CoordinateType maxValue = 80.0;
		const CircleEngine::CoordinateType maxVelValue = 30.0;
		
		if (i == 0)
		{
			objContainer->Color = CircleEngine::CircleCoordinator::ObjectColor(1, 1, 0);
			objContainer->Detal = 20;
			obj->Radius = big_radius;
			obj->Weight = 53.0;
		}
		else
		{
			objContainer->Color = CircleEngine::CircleCoordinator::ObjectColor(0.6, 0, 0);			
			objContainer->Detal = 6;
			obj->Center = CircleEngine::Point(rand_pmmax(maxValue), rand_pmmax(maxValue), rand_pmmax(maxValue));  // 
			obj->Velocity = CircleEngine::Point(rand_pmmax(maxVelValue), rand_pmmax(maxVelValue), 0); // CircleEngine::Point(0,0,0);
			obj->Radius = .3 + rand_pmmax(.2);
			obj->Weight = obj->Radius * obj->Radius * obj->Radius / 8;		
		}
		
		g_CircleCoordinator.AddObject(objContainer);
	}

	std::vector<CircleEngine::CircleCoordinator::ObjectPtr> objectConts = g_CircleCoordinator.GetObjects();
	for (size_t i = 0; i < objectConts.size(); i++)
	{
		CircleEngine::CircleObjectPtr obj = objectConts[i]->Obj;
		
		allSeqSelector->Add(obj);
		allCrossNearSelector->Add(obj);
		allCrossSelector->Add(obj);
		if (obj->Radius == big_radius)
			oneToOtherSelector->AddSome(obj);
		else
			oneToOtherSelector->AddOther(obj);
	}

	CircleEngine::CoordinateType accuracy = 0.005;
	CircleEngine::RulePtr moveRule(new CircleEngine::RuleMove(allSeqSelector, accuracy));
	CircleEngine::RulePtr contactRule(new CircleEngine::RuleContact(allCrossNearSelector));
	CircleEngine::RulePtr gravityRule(new CircleEngine::RuleGravity(oneToOtherSelector, 5000, accuracy));
	CircleEngine::RulePtr strongBarRule(new CircleEngine::RuleStrongBar(pairBarSelector));
	
	g_CircleCoordinator.AddRule(moveRule);
	g_CircleCoordinator.AddRule(strongBarRule);
	g_CircleCoordinator.AddRule(gravityRule);
	g_CircleCoordinator.AddRule(strongBarRule);
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
