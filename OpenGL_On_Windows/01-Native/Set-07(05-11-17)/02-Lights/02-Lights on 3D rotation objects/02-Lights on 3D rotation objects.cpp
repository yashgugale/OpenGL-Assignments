#include <windows.h>
#include <gl/GL.h>
#include<gl/GLU.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
//Prototype Of WndProc() declared Globally
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global variable declarations
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullscreen = false;

float angle_Pyramid = 0.0f;
float angle_Cube = 0.0f;

bool gbLighting = false;
GLUquadric *quadric = NULL;

//Lighting arrays:
GLfloat light_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_position[] = { 0.0f, 0.0f, 1.0f, 0.0f };
//GLfloat material_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//GLfloat material_shininess[] = { 50.0f };

//main()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//function prototype
	void initialize(void);
	void uninitialize(void);
	void display(void);
	void update(void);

	//variable declaration
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("RTROGL");
	bool bDone = false;

	int xPos, yPos;
	MONITORINFO mi;
	mi = { sizeof(MONITORINFO) };

	//code

	//initializing members of struct WNDCLASSEX
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.lpfnWndProc = WndProc;
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;

	//Registering Class
	RegisterClassEx(&wndclass);

	//Create Window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("Rotating Objects with lights"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		0,
		0,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	//initialize
	initialize();

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	GetMonitorInfo(MonitorFromWindow(hwnd, MONITORINFOF_PRIMARY), &mi);
	xPos = (mi.rcMonitor.right - mi.rcMonitor.left) / 2;
	yPos = (mi.rcMonitor.bottom - mi.rcMonitor.top) / 2;
	SetWindowPos(hwnd, HWND_TOP, xPos - 400, yPos - 300, 800, 600, SWP_NOZORDER | SWP_FRAMECHANGED);



	//Message Loop
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == true)
			{
				update(); 
				display();
				if (gbEscapeKeyIsPressed == true)
					bDone = true;
			}
		}
	}

	uninitialize();
	return((int)msg.wParam);
}

//WndProc()
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//function prototype
	//	void display(void);
	void resize(int, int);
	void ToggleFullscreen(void);
	void uninitialize(void);

	//code
	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;
	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			gbEscapeKeyIsPressed = true;
			break;
		case 0x46: //for 'f' or 'F'
			if (gbFullscreen == false)
			{
				ToggleFullscreen();
				gbFullscreen = true;
			}
			else
			{
				ToggleFullscreen();
				gbFullscreen = false;
			}
			break;
		case 0x4C:	//For 'l' or 'L'
			if (gbLighting == false)
			{
				gbLighting = true;
				glEnable(GL_LIGHTING);
			}
			else
			{
				gbLighting = false;
				glDisable(GL_LIGHTING);
			}
			break;
		default:
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void)
{
	//variable declarations
	MONITORINFO mi;

	//code
	if (gbFullscreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}

	else
	{
		//code
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
	}
}

void initialize(void)
{
	//function prototypes
	void resize(int, int);

	//variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	//code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	//Initialization of structure 'PIXELFORMATDESCRIPTOR'
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	ghdc = GetDC(ghwnd);

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glClearDepth(1.0f);			//clear depth buffer
	glEnable(GL_DEPTH_TEST);	//enable the depth
	glDepthFunc(GL_LEQUAL);		//less than or equal to 1.0f in far. Uses ray tracing algorithm

	//Optional Calls
	glShadeModel(GL_SMOOTH);				//to remove aliasing
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	//to remove distortion

	//Lighting calls
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	//glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	//glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);
	
	//Enable LIGHT0
	glEnable(GL_LIGHT0);

	//Enable color material to keep the colors from glColor3f()
	//glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	//glEnable(GL_COLOR_MATERIAL);

	resize(WIN_WIDTH, WIN_HEIGHT);
}

void display(void)
{
	void multicolored_pyramid(void);
	void multicolored_cube(void);

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glTranslatef(-1.5f, 0.0f, -6.0f);
	glRotatef(angle_Pyramid, 0.0f, 1.0f, 0.0f);	//Rotate the pyramid about y axis
	multicolored_pyramid();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(1.5f, 0.0f, -6.0f);
	glScalef(0.75f, 0.75f, 0.75f);
	glRotatef(angle_Cube, 1.0f, 0.0f, 0.0f);	//Rotate the cube about x axes
	glRotatef(angle_Cube, 0.0f, 1.0f, 0.0f);	//Rotate the cube about y axes
	glRotatef(angle_Cube, 0.0f, 0.0f, 1.0f);	//Rotate the cube about z axes
	multicolored_cube();

	SwapBuffers(ghdc);

}

void resize(int width, int height)
{
	//code
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);	//Set angle = 0.0f to view the triangle withour translation


}

void uninitialize(void)
{
	//UNINITIALIZATION CODE

	if (gbFullscreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);

	}

	wglMakeCurrent(NULL, NULL);

	wglDeleteContext(ghrc);
	ghrc = NULL;

	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	DestroyWindow(ghwnd);
	ghwnd = NULL;
}

void multicolored_pyramid(void)
{

	glBegin(GL_TRIANGLES);
	//FRONT FACE
	glNormal3f(0.0f, 0.0f, 1.0f);	//Normal
	glColor3f(1.0f, 0.0f, 0.0f); //red
	glVertex3f(0.0f, 1.0f, 0.0f); //apex

	glColor3f(0.0f, 1.0f, 0.0f); //green
	glVertex3f(-1.0f, -1.0f, 1.0f); //left-corner of front face

	glColor3f(0.0f, 0.0f, 1.0f); //blue
	glVertex3f(1.0f, -1.0f, 1.0f); //right-corner of front face

	//RIGHT FACE
	glNormal3f(1.0f, 0.0f, 0.0f);	//Normal
	glColor3f(1.0f, 0.0f, 0.0f); //red
	glVertex3f(0.0f, 1.0f, 0.0f); //apex

	glColor3f(0.0f, 0.0f, 1.0f); //blue
	glVertex3f(1.0f, -1.0f, 1.0f); //left-corner of right face

	glColor3f(0.0f, 1.0f, 0.0f); //green
	glVertex3f(1.0f, -1.0f, -1.0f); //right-corner of right face

	//BACK FACE
	glNormal3f(0.0f, 0.0f, -1.0f);	//Normal
	glColor3f(1.0f, 0.0f, 0.0f); //red
	glVertex3f(0.0f, 1.0f, 0.0f); //apex

	glColor3f(0.0f, 1.0f, 0.0f); //green
	glVertex3f(1.0f, -1.0f, -1.0f); //left-corner of back face

	glColor3f(0.0f, 0.0f, 1.0f); //blue
	glVertex3f(-1.0f, -1.0f, -1.0f); //right-corner of back face

	//LEFT FACE
	glNormal3f(-1.0f, 0.0f, 0.0f);	//Normal
	glColor3f(1.0f, 0.0f, 0.0f); //red
	glVertex3f(0.0f, 1.0f, 0.0f); //apex

	glColor3f(0.0f, 0.0f, 1.0f); //blue
	glVertex3f(-1.0f, -1.0f, -1.0f); //left-corner of left face

	glColor3f(0.0f, 1.0f, 0.0f); //green
	glVertex3f(-1.0f, -1.0f, 1.0f); //right-corner of left face
	glEnd();

}

void multicolored_cube(void)
{

	//glLineWidth(1.0f);
	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.0f, 0.0f);	//Red face
	//Top face
	glNormal3f(0.0f, 1.0f, 0.0f);	//Normal
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);

	glColor3f(0.0f, 1.0f, 0.0f);	//Green
	//Bottom face
	glNormal3f(0.0f, -1.0f, 0.0f);	//Normal
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);


	glColor3f(0.0f, 0.0f, 1.0f);	//Blue
	//Front face
	glNormal3f(0.0f, 0.0f, 1.0f);	//Normal
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	glColor3f(0.0f, 1.0f, 1.0f);	//Cyan
	//Back face
	glNormal3f(0.0f, 0.0f, -1.0f);	//Normal
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	glColor3f(1.0f, 0.0f, 1.0f);	//Magenta
	//Right face
	glNormal3f(1.0f, 0.0f, 0.0f);	//Normal
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	glColor3f(1.0f, 1.0f, 0.0f);	//Yellow
	//Left face
	glNormal3f(-1.0f, 0.0f, 0.0f);	//Normal
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	glEnd();

}

void update(void)
{

	//code
	angle_Pyramid = angle_Pyramid + 0.05f;
	if (angle_Pyramid >= 360.0f)
		angle_Pyramid = 0.0f;

	angle_Cube = angle_Cube - 0.05f;
	if (angle_Cube <= -360.0f)
		angle_Cube = 0.0f;
}
