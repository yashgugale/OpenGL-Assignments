#include <windows.h>
#include<windowsx.h>
#include <gl/GL.h>
#include<gl/GLU.h>
#include<math.h>
#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define PI 3.1415

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

//int timer = 0;
int i = 0;

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
		TEXT("Happy Birthday Vedika"),
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
				display();
				Sleep(300);
				//update();
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

	resize(WIN_WIDTH, WIN_HEIGHT);
}

void display(void)
{

	void drawCircle(GLfloat, GLfloat, GLfloat);
	void Draw_H(void);
	void Draw_A(void);
	void Draw_P(void);
	void Draw_Y(void);
	void Draw_B(void);
	void Draw_I(void);
	void Draw_R(void);
	void Draw_T(void);
	void Draw_D(void);
	void Draw_V(void);
	void Draw_E(void);
	void Draw_K(void);


	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//glTranslatef(-2.0f, 0.0f, -4.5f);
	//drawCircle(0.0f, 0.0f, 0.2f);
	//glTranslatef(4.0f, 0.0f, 0.0f);
	//drawCircle(0.0f, 0.0f, 0.2f);
	
	glLoadIdentity();
	glTranslatef(-1.5f, 1.3f, -4.5f);
	Draw_H();
	glLoadIdentity();
	glTranslatef(-0.5f, 1.3f, -4.5f);
	Draw_A();
	glLoadIdentity();
	glTranslatef(0.0f, 1.3f, -4.5f);
	Draw_P();
	glLoadIdentity();
	glTranslatef(0.6f, 1.3f, -4.5f);
	Draw_P();
	glLoadIdentity();
	glTranslatef(1.2f, 1.3f, -4.5f);
	Draw_Y();

	glLoadIdentity();
	glTranslatef(-2.5f, 0.0f, -4.5f);
	Draw_B();
	glLoadIdentity();
	glTranslatef(-2.0f, 0.0f, -4.5f);
	Draw_I();
	glLoadIdentity();
	glTranslatef(-1.2f, 0.0f, -4.5f);
	Draw_R();
	glLoadIdentity();
	glTranslatef(-0.6f, 0.0f, -4.5f);
	Draw_T();
	glLoadIdentity();
	glTranslatef(0.2f, 0.0f, -4.5f);
	Draw_H();
	glLoadIdentity();
	glTranslatef(1.0f, 0.0f, -4.5f);
	Draw_D();
	glLoadIdentity();
	glTranslatef(2.0f, 0.0f, -4.5f);
	Draw_A();
	glLoadIdentity();
	glTranslatef(2.3f, 0.0f, -4.5f);
	Draw_Y();

	glLoadIdentity();
	glTranslatef(-1.9f, -1.2f, -4.5f);
	Draw_V();
	glLoadIdentity();
	glTranslatef(-1.2f, -1.2f, -4.5f);
	Draw_E();
	glLoadIdentity();
	glTranslatef(-0.45f, -1.2f, -4.5f);
	Draw_D();
	glLoadIdentity();
	glTranslatef(0.15f, -1.2f, -4.5f);
	Draw_I();
	glLoadIdentity();
	glTranslatef(0.9f, -1.2f, -4.5f);
	Draw_K();
	glLoadIdentity();
	glTranslatef(1.75f, -1.2f, -4.5f);
	Draw_A();

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
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

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




void drawCircle(GLfloat x, GLfloat y, GLfloat radius)
{
	GLint circle_points = 10000; // we can also give 100, 1000, 10000
	GLfloat angle;

	glLineWidth(5.0f);
	glEnable(GL_LINE_SMOOTH);	//smoothens out the line
	glBegin(GL_LINE_STRIP);
	glColor3f(0.8352f, 0.0588f, 0.1450f);	//RED
	for (int i = 0; i < circle_points; i++)
	{
		glVertex2f(x, y);
		angle = 2.0f * (PI)* i / circle_points;
		glVertex2f(x + (radius*cos(angle)), (radius*sin(angle))); //value is added to angle to move the arc along the circle
	}
	glEnd();
}

void Draw_H(void)
{
	void multicolor(void);
	glBegin(GL_LINES);
	multicolor();
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.5f, 0.0f, 0.0f);
	glEnd();
}

void Draw_A(void)
{
	void multicolor(void);
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	multicolor();
	glVertex3f(-0.3f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.3f, -0.5f, 0.0f);
	glVertex3f(-0.17f, -0.1f, 0.0f);
	glVertex3f(0.17f, -0.1f, 0.0f);
	glEnd();
}

void Draw_B(void)
{
	void multicolor(void);
	glBegin(GL_LINES);
	multicolor();
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.2f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.2f, -0.5f, 0.0f);
	glEnd();

	glRotatef(270.0f, 0.0f, 0.0f, 1.0f);

	glPointSize(3.0f);
	glBegin(GL_POINTS);
	GLfloat fRadius_For_B = 0.25f;
	for (GLfloat angle = 0.0f; angle < 1.0f*PI; angle = angle + 0.01f)
	{
		glVertex3f(fRadius_For_B*cos(angle) - 0.24f, fRadius_For_B*sin(angle) + 0.2f, 0.0f);
	}
	glEnd();

	glBegin(GL_POINTS);
	for (GLfloat angle = 0.0f; angle < 1.0f*PI; angle = angle + 0.01f)
	{
		glVertex3f(fRadius_For_B*cos(angle) + 0.24f, fRadius_For_B*sin(angle) + 0.2f, 0.0f);
	}
	glEnd();
}

void Draw_D(void)
{
	void multicolor(void);
	glBegin(GL_LINES);
	multicolor();
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.1f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.1f, -0.5f, 0.0f);
	glEnd();

	glRotatef(270.0f, 0.0f, 0.0f, 1.0f);

	glPointSize(3.0f);
	glBegin(GL_POINTS);
	GLfloat fRadius_For_D = 0.5f;
	for (GLfloat angle = 0.0f; angle < 1.0f*PI; angle = angle + 0.01f)
	{
		glVertex3f(fRadius_For_D*cos(angle), fRadius_For_D*sin(angle) + 0.1f, 0.0f);
	}
	glEnd();
}

void Draw_E(void)
{
	void multicolor(void);
	glBegin(GL_LINES);
	multicolor();
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.35f, 0.0f, 0.0f);
	glEnd();
}

void Draw_I(void)
{
	void multicolor(void);
	glBegin(GL_LINES);
	multicolor();
	glVertex3f(0.25f, 0.5f, 0.0f);
	glVertex3f(0.25f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glEnd();
}

void Draw_K(void)
{
	void multicolor(void);
	glBegin(GL_LINES);
	multicolor();
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.0f, -0.1f, 0.0f);
	glVertex3f(0.4f, 0.5f, 0.0f);
	glVertex3f(0.1f, 0.03f, 0.0f);
	glVertex3f(0.4f, -0.5f, 0.0f);
	glEnd();
}

void Draw_P(void)
{
	void multicolor(void);
	glBegin(GL_LINES);
	multicolor();
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.2f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.01f, 0.0f);
	glVertex3f(0.2f, -0.01f, 0.0f);
	glEnd();

	glRotatef(270.0f, 0.0f, 0.0f, 1.0f);

	glPointSize(3.0f);
	glBegin(GL_POINTS);
	GLfloat fRadius_For_P = 0.25f;
	for (GLfloat angle = 0.0f; angle < 1.0f*PI; angle = angle + 0.01f)
	{
		glVertex3f(fRadius_For_P*cos(angle) - 0.24f, fRadius_For_P*sin(angle) + 0.2f, 0.0f);
	}
	glEnd();
}

void Draw_R(void)
{
	void multicolor(void);
	glBegin(GL_LINES);
	multicolor();
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.2f, 0.5f, 0.0f);
	glVertex3f(0.0f, -0.01f, 0.0f);
	glVertex3f(0.2f, -0.01f, 0.0f);
	glVertex3f(0.0f, -0.01f, 0.0f);
	glVertex3f(0.4f, -0.5f, 0.0f);
	glEnd();

	glRotatef(270.0f, 0.0f, 0.0f, 1.0f);

	glPointSize(3.0f);
	glBegin(GL_POINTS);
	GLfloat fRadius_For_R = 0.25f;
	for (GLfloat angle = 0.0f; angle < 1.0f*PI; angle = angle + 0.01f)
	{
		glVertex3f(fRadius_For_R*cos(angle) - 0.24f, fRadius_For_R*sin(angle) + 0.2f, 0.0f);
	}
	glEnd();
}

void Draw_T(void)
{
	void multicolor(void);
	glBegin(GL_LINES);
	multicolor();
	glVertex3f(0.25f, 0.5f, 0.0f);
	glVertex3f(0.25f, -0.5f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glEnd();
}

void Draw_V(void)
{
	void multicolor(void);
	glBegin(GL_LINES);
	multicolor();
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.25f, -0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0.25f, -0.5f, 0.0f);
	glEnd();
}

void Draw_Y(void)
{
	void multicolor(void);
	glPointSize(3.0f);
	glBegin(GL_LINES);
	multicolor();
	glVertex3f(0.0f, 0.5f, 0.0f);
	glVertex3f(0.25f, 0.0f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0.25f, 0.0f, 0.0f);
	glVertex3f(0.25f, 0.0f, 0.0f);
	glVertex3f(0.25f, -0.5f, 0.0f);
	glEnd();
}

void multicolor(void)
{
	if (i % 10 == 0)
	{
		glColor3f(0.0f, 1.0f, 1.0f);	//Cyan
	}

	if (i % 10 == 1)
	{
		glColor3f(1.0f, 0.5f, 0.0f);	//Yellow
	}
	if (i % 10 == 2)
	{
		glColor3f(1.0f, 1.0f, 1.0f);	//White
	}
	if (i % 10 == 3)
	{
		glColor3f(1.0f, 0.0f, 0.0f);	//RED
	}
	if (i % 10 == 4)
	{
		glColor3f(0.5f, 1.0f, 0.5f);	//Light Green
	}
	if (i % 10 == 5)
	{
		glColor3f(0.737255f, 0.560784f, 0.560784f);	//Pink
	}
	if (i % 10 == 6)
	{
		glColor3f(1.0f, 0.0f, 1.0f);	//Purple
	}
	if (i % 10 == 7)
	{
		glColor3f(0.0f, 0.0f, 1.0f);	//BlUE
	}
	if (i % 10 == 8)
	{
		glColor3f(0.0f, 1.0f, 0.0f);	//Green
	}
	if (i % 10 == 9)
	{
		glColor3f(0.647059f, 0.164706f, 0.164706f);	//Brown
	}
	i++;
}

void update(void)
{
	i++;
}