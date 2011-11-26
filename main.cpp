#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "CircleWorld.h"
#include <vector>

const CoordinateType accuraty = 0.01;

void Gravity(CircleObject* a_CircleObject)
{
	if (a_CircleObject->IsFixed)
		return;
		
	Point centrGravity(0.0, 0.0, 0.0);
	Point& circleCentr = a_CircleObject->Center;

	CoordinateType dist = centrGravity.Distance(circleCentr);
	CoordinateType onedivdist = 0.0;
	if (dist > 0.0)
		onedivdist = 1.0 / dist;

	if (dist > 0.1)
	{
		Point accelerationVector = (centrGravity - circleCentr) * onedivdist;
		a_CircleObject->Velocity = a_CircleObject->Velocity + accelerationVector * onedivdist * onedivdist * 250.0 * accuraty;
	}
}

void resize(int width,int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (height)
		gluPerspective(30.0f, width / height, 0.1f, 1500.0f);
	//glOrtho(-10,10, -10,10, -5.02,120);   
	gluLookAt(0,0,50, 0,0,0, 0,1,0);
	glMatrixMode(GL_MODELVIEW);
}    

std::vector<CircleObject> objects;

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

void display(void)
{
	CalculateNextPosition(&objects);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (size_t i = 0; i < objects.size(); i++)
	{
		glPushMatrix();
		
		CircleObject& obj = objects[i];
		glTranslated(obj.Center.x, obj.Center.y, obj.Center.z);
		if (i == 0)
		{
			glColor3d(1, 1, 0);
			glutSolidSphere(obj.Radius, 20, 20);
		}	
		else
		{
			glColor3d(0.6, 0.0, 0.1);
			glutSolidSphere(obj.Radius, 10, 10);
		}
		
		glPopMatrix();
	}
	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	float pos[4] = {5,5,5,1};
	float dir[3] = {-1,-1,-1};
	GLfloat mat_specular[] = {1,1,1,1};

	glutInit(&argc, argv);

	glutInitWindowPosition(50, 10);
	glutInitWindowSize(800, 600);
	glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_ACCUM );
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

	CircleObject obj;
	obj.Center = Point(0 , 0 , 0);
	obj.Velocity = Point(0, 0, 0);
	obj.Radius = 2.0;
	obj.IsFixed = true;
	objects.push_back(obj);

	for (size_t i = 0; i < 5000; i++)
	{
		#define rand_pmmax(maxValue) (maxValue * rand() / (RAND_MAX * 1.0) - (maxValue) / 2.0)
		CircleObject obj;
		const CoordinateType maxValue = 80.0;
		const CoordinateType maxVelValue = 20.0;
		obj.Center = Point(rand_pmmax(maxValue), rand_pmmax(maxValue), 0.0);
		obj.Velocity = Point(rand_pmmax(maxVelValue), rand_pmmax(maxVelValue), 0);
		obj.Radius = 0.2;// + rand_pmmax(.18);

		objects.push_back(obj);
	}

    glutMainLoop();
	return 0;
}
