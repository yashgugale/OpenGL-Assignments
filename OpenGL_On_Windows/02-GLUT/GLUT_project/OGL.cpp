#include<GL/freeglut.h>
#include<math.h>

//global variable declarations
bool isFullScreen = false;	//variable to toggle fullscreen
static int day = 0;
static int year = 0;
int main(int argc, char **argv)
{
	//function prototypes
	void display(void);
	void resize(int, int);
	void keyboard(unsigned char, int, int);
	void mouse(int, int, int, int);
	void initialize(void);
	void uninitialize(void);

	//code

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) / 2) - 400, (glutGet(GLUT_SCREEN_HEIGHT) / 2) - 300); //get height and width of screen and then find center coordinates
	glutCreateWindow("GLUT-Solar System");

	initialize();

	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutCloseFunc(uninitialize);
	glutMainLoop();

	//	return(0);
}

void display(void)
{

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//glTranslatef(0.0f, 0.0f, -12.0f);
	gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glPushMatrix();
	//Draw sun
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);	//Rotate it by 90 to change the orientation
	glColor3f(1.0f, 1.0f, 0.0f);
	glutWireSphere(1.0f, 30,30);
	glRotatef((GLfloat)year, 0.0f, 0.0f, 1.0f);	//As we have rotated the sphere by 90, its local co-ordinate sytem changes
												//Hence, we need to rotate about z axis so that we get normal revolution
	glTranslatef(2.0f, 0.0f, 0.0f);		//We translate by 2 here
	glRotatef((GLfloat)day, 0.0f, 0.0f, 1.0f);
	glColor3f(0.4f, 0.9f, 1.0f);
	glutWireSphere(0.2f, 20, 20);	//size of sphere, no of slices(longitude), no of stack(latitude)

	glPopMatrix();
	glutSwapBuffers();
}



void initialize(void)
{
	//code

	//to select clearing(background) clear
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

void keyboard(unsigned char key, int x, int y)
{
	//code

	switch (key)
	{
	case 27:	//Escape
		glutLeaveMainLoop();
		break;
	case 'F':
	case 'f':
		if (isFullScreen == false)
		{
			glutFullScreen();
			isFullScreen = true;

		}
		else
		{
			glutLeaveFullScreen();
			isFullScreen = false;

		}
		break;
	case 'D':					//Day
		day = (day + 6) % 360;	//for Rotation of earth about its own axis
		glutPostRedisplay();
		break;
	case 'd':
		day = (day - 6) % 360;
		glutPostRedisplay();
		break;
	case 'Y':					//Year
		year = (year + 3) % 360;//For revolution of earth about the sun
		glutPostRedisplay();
		break;
	case 'y':
		year = (year - 3) % 360;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

void mouse(int button, int state, int x, int y)
{
	//code

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		break;
	default:
		break;
	}
}

void resize(int width, int height)
{
	//code
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}

void uninitialize(void)
{
	//code
}
