#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "CircleWorld.h"
#include <vector>

void Gravity(CircleObject* a_CircleObject)
{
	Point centrGravity(0.0, 0.0, 0.0);
	Point& circleCentr = a_CircleObject->Center;

	CoordinateType dist = centrGravity.Distance(circleCentr);
	CoordinateType onedivdist = 0.0;
	Point accelerationVector;
	if (dist > 0.0)
		onedivdist = 1.0 / dist;

	if (dist > 1.0)
	{
		accelerationVector = (centrGravity - circleCentr) * onedivdist;
		a_CircleObject->Velocity = a_CircleObject->Velocity + accelerationVector * onedivdist * 1.5;
	}
}

void resize(int width,int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-5,5, -5,5, 2,12);   
	gluLookAt(0,0,10, 0,0,0, 0,1,0);
	glMatrixMode(GL_MODELVIEW);
}    

std::vector<CircleObject> objects;

void display(void)
{
	const CoordinateType accuraty = 0.001;
	
	for (size_t i1 = 0; i1 < objects.size(); i1++)
	{
		CircleObject& obj1 = objects[i1];
		
		Gravity(&obj1);

		obj1.Center.x += obj1.Velocity.x * accuraty;
		obj1.Center.y += obj1.Velocity.y * accuraty;
		obj1.Center.z += obj1.Velocity.z * accuraty;
		
		for (size_t i2 = i1; i2 < objects.size(); i2++)
		{
			if (i1 == i2)
				continue;

			CircleObject& obj2 = objects[i2];

			if (obj1.IsIntersection(obj2))
			{
				obj1.ResolveContact(&obj2);
			}
		}
	}
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (size_t i = 0; i < objects.size(); i++)
	{
		glPushMatrix();
		
		CircleObject& obj = objects[i];
		glTranslated(obj.Center.x, obj.Center.y, obj.Center.z);
		glColor3d(1, 0, 0);
		glutSolidSphere(obj.Radius, 10, 10);
		
		glPopMatrix();
	}
	glRotated(0.01, 0, 0, 1); 
	
	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	float pos[4] = {3,3,3,1};
	float dir[3] = {-1,-1,-1};
	GLfloat mat_specular[] = {1,1,1,1};

	glutInit(&argc, argv);

	glutInitWindowPosition(50, 10);
	glutInitWindowSize(800, 600);
	glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE );
	glutCreateWindow("GLUT Template");
	glutIdleFunc(display);
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
  
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, 128.0);

	for (size_t i = 0; i < 200; i++)
	{
		CircleObject obj;
		const CoordinateType maxValue = 1.0;
		const CoordinateType maxVelValue = 0.0;
		obj.Center = Point(maxValue * rand() / (RAND_MAX * 1.0), maxValue * rand() / (RAND_MAX * 1.0), maxValue * rand() / (RAND_MAX * 1.0));
		obj.Velocity = Point(maxVelValue * rand() / (RAND_MAX * 1.0), maxVelValue * rand() / (RAND_MAX * 1.0), maxVelValue * rand() / (RAND_MAX * 1.0));
		obj.Radius = 0.15;

		objects.push_back(obj);
	}

    glutMainLoop();
	return 0;
}
