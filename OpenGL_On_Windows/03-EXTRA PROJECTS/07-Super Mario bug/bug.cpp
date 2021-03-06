#include <windows.h>
#include<windowsx.h>
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

int timer = 0;

//main()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//function prototype
	void initialize(void);
	void uninitialize(void);
	void display(void);

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
		TEXT("OpenGL : Super Mario Bug"),
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

	glClearColor(0.38823529411764707f, 0.5254901960784314f, 0.9803921568627451f, 0.0f);

	resize(WIN_WIDTH, WIN_HEIGHT);
}

void display(void)
{

	void supermario(void);

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -4.5f);
	supermario();

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


void supermario(void)
{
	void red(void);
	void green(void);
	void yellow(void);

	//Drawing Super mario bug
	//Line 1
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-5.0f, 15.0f, -60.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();

	//Line 2
	glTranslatef(-8.0f, -2.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();

	//Line 3
	glTranslatef(-12.0f, -2.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();

	//Line 4
	glTranslatef(-16.0f, -2.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();

	//Line 5
	glTranslatef(-20.0f, -2.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();

	//Line 6
	glTranslatef(-24.0f, -2.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();

	//Line 7
	glTranslatef(-26.0f, -2.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();

	//Line 8
	glTranslatef(-28.0f, -2.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();

	//Line 9
	glTranslatef(-30.0f, -2.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();


	//Line 10
	glTranslatef(-30.0f, -2.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();

	//Line 11
	glTranslatef(-28.0f, -2.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();
	glTranslatef(2.0f, 0.0f, 0.0f);
	red();


	//Line 12
	glTranslatef(-20.0f, -2.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();

	//Line 13
	glTranslatef(-18.0f, -2.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();

	//Line 14
	glTranslatef(-20.0f, -2.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();


	//Line 15
	glTranslatef(-22.0f, -2.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();

	//Line 16
	glTranslatef(-20.0f, -2.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	yellow();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
	glTranslatef(2.0f, 0.0f, 0.0f);
	green();
}

void red(void)
{
	glBegin(GL_QUADS);
	glColor3f(0.615686274509804, 0.27058823529411763, 0.0196078431372549);	//Brown
	glVertex3f(1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);

	glEnd();
}

void green(void)
{
	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 0.0f);	//Black
	glVertex3f(1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);

	glEnd();
}

void yellow(void)
{
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.7490196078431373, 0.5764705882352941);	//Skin
	glVertex3f(1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);

	glEnd();
}

/*
GLfloat gfColor_Offwhite_R = 0.997f;
GLfloat gfColor_Offwhite_G = 0.92f;
GLfloat gfColor_Offwhite_B = 0.84f;
*/