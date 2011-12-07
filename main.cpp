#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <string>
#include <sstream>

#include "CircleCoordinator.h"
#include "GetWorkTime.h"

void resize(int width,int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (height)
		gluPerspective(30.0f, width / height, 0.1f, 1500.0f);
	gluLookAt(0,-150,20, 0,0,0, 0,1,0);
	glMatrixMode(GL_MODELVIEW);
}    

CircleEngine::CircleCoordinator g_CircleCoordinator;
GetWorkTime		g_WorkTime;

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 97: // 'a'
			glRotated(-1, 0, 0, 1); 			
			break;
		case 100: // 'd'
			glRotated(1, 0, 0, 1); 			
			break;
		case 119: // 'w'
			glRotated(-1, 1, 0, 0); 			
			break;
		case 115: // 's'
			glRotated(1, 1, 0, 0); 			
			break;
	}
}

void DebugOtput(const std::string& string)
{
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
	glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
	glRasterPos2i(-10, 50);
	glColor3d(1.0, 1.0, 1.0);
	for(int i =0; i < string.size(); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}  
	glEnable(GL_LIGHTING);
	glPopAttrib();
}

void display(void)
{
	g_CircleCoordinator.DoStep();

	std::vector<CircleEngine::CircleObjectPtr> objects = g_CircleCoordinator.GetObjects();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (size_t i = 0; i < objects.size(); i++)
	{
		glPushMatrix();
		
		CircleEngine::CircleObjectPtr obj = objects[i];
		glTranslated(obj->Center.x, obj->Center.y, obj->Center.z);
		if (i == 0)
		{
			glColor3d(1, 1, 0);
			glutSolidSphere(obj->Radius, 20, 20);
		}	
		else
		{
			glColor3d(0.6, 0.0, 0.1);
			glutSolidSphere(obj->Radius, 7, 7);
		}
		
		glPopMatrix();
	}
	
	static size_t s_executecount = 0;
	s_executecount++;
	std::stringstream sstr;
	sstr << "fps " << s_executecount / g_WorkTime.GetCurrentTime();
	DebugOtput(sstr.str());
	
	if (s_executecount > 100)
	{
		g_WorkTime.Start();
		s_executecount = 0;
	}	

	glutSwapBuffers();
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
	
	glutIdleFunc(display);
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
  
	glShadeModel(GL_SMOOTH);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, 128.0);

	CircleEngine::CircleObjectPtr obj(new CircleEngine::CircleObject);
	obj->Radius = 2.0;
	obj->Weight = 1.0;
	g_CircleCoordinator.AddObject(obj);

	for (size_t i = 0; i < 5000; i++)
	{
		#define rand_pmmax(maxValue) (maxValue * rand() / (RAND_MAX * 1.0) - (maxValue) / 2.0)
		CircleEngine::CircleObjectPtr obj(new CircleEngine::CircleObject);
		const CircleEngine::CoordinateType maxValue = 80.0;
		const CircleEngine::CoordinateType maxVelValue = 20.0;
		obj->Center = CircleEngine::Point(rand_pmmax(maxValue), rand_pmmax(maxValue), 0);
		obj->Velocity = CircleEngine::Point(rand_pmmax(maxVelValue), rand_pmmax(maxVelValue), 0); // CircleEngine::Point(0,0,0);
		obj->Radius = 0.2 + rand_pmmax(.1);
		obj->Weight = obj->Radius * obj->Radius * obj->Radius / 8;
		
		g_CircleCoordinator.AddObject(obj);
	}
	
	std::vector<CircleEngine::CircleObjectPtr> objects = g_CircleCoordinator.GetObjects();
	CircleEngine::SequenceSelectorPtr allSeqSelector(new CircleEngine::SequenceSelector); 
	CircleEngine::CrossNearSelectorPtr allCrossNearSelector(new CircleEngine::CrossNearSelector(10)); 
	CircleEngine::SomeToOtherSelectorPtr oneToOtherSelector(new CircleEngine::SomeToOtherSelector); 
	for (size_t i = 0; i < objects.size(); i++)
	{
		allSeqSelector->Add(objects[i]);
		allCrossNearSelector->Add(objects[i]);
		if (i == 0)
			oneToOtherSelector->AddSome(objects[i]);
		else
			oneToOtherSelector->AddOther(objects[i]);
	}

	CircleEngine::CoordinateType accuracy = 0.005;
	
	CircleEngine::RulePtr moveRule(new CircleEngine::RuleMove(allSeqSelector, accuracy));
	CircleEngine::RulePtr contactRule(new CircleEngine::RuleContact(allCrossNearSelector));
	CircleEngine::RulePtr gravityRule(new CircleEngine::RuleGravity(oneToOtherSelector, 5000, accuracy));
	
	g_CircleCoordinator.AddRule(moveRule);
	g_CircleCoordinator.AddRule(gravityRule);
	g_CircleCoordinator.AddRule(contactRule);
		
	g_WorkTime.Start();
	
    glutMainLoop();
	return 0;
}
