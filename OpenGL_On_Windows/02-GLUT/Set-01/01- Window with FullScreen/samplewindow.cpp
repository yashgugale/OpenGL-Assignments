#include<GL/freeglut.h>

//global variable declarations
bool isFullScreen = false;	//variable to toggle fullscreen

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
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)/2)-400,(glutGet(GLUT_SCREEN_HEIGHT)/2)-300);
	glutCreateWindow("GLUT Applications");

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

	//to clear all pixels
	glClear(GL_COLOR_BUFFER_BIT);
	





	glutSwapBuffers();
}

void initialize(void)
{
	//code

	//to select clearing(background) clear
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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
}

void uninitialize(void)
{
	//code
}