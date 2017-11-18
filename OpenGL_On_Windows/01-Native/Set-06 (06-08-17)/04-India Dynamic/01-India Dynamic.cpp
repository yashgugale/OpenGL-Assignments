//Windows headers
#include <windows.h>

//header for using sound function
#include<windowsx.h>

//OpenGL headers
#include <gl/GL.h>
#include<gl/GLU.h>

#define WIN_WIDTH 800		//Width of the window
#define WIN_HEIGHT 600		//Height of the window

//Linking the libraries
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"Winmm.lib")

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

//variables for animation
int timer = 1;
GLfloat I1_movement = -1.3f;
GLfloat N_movement = 2.5f;
GLfloat D_fade = 0.0f;
GLfloat Red = 0.0f;
GLfloat Green1 = 0.0f;
GLfloat Green2 = 0.0f;
GLfloat I2_movement = -2.0f;
GLfloat A_movement = 1.8f;
GLfloat orange_left = -3.5f;
GLfloat white_left = -3.5f;
GLfloat green_left = -3.5f;

GLfloat tricolor_movement = -2.2f;
//main()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//function prototype
	void initialize(void);
	void uninitialize(void);
	void display(void);
	void Update(void);

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
		TEXT("OpenGL : Dynamic India"),
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

	//Position window to the center of the screen
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
				Update();	//All animations here
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
	void resize(int, int);
	void ToggleFullscreen(void);
	void uninitialize(void);

	//code
	switch (iMsg)
	{
	case WM_CREATE:
		//Playing National Anthem Song on window creation
		PlaySound(TEXT("NA.wav"), NULL, SND_FILENAME | SND_ASYNC);
		break;
	case WM_ACTIVATE:					//Current active window
		if (HIWORD(wParam) == 0)
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;
	case WM_SIZE:						//Resize the window
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_KEYDOWN:					//Handle all key press events
		switch (wParam)
		{
		case VK_ESCAPE:					//Exit on Esc key
			gbEscapeKeyIsPressed = true;
			break;
		case 0x46: 					//for 'f' or 'F'- Toggle fullscreen.	
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
	case WM_DESTROY:					//Destroy window
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

//Function for fullscreen
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

	//Initialization of structure 'PIXELFORMATDESCRIPTOR' and set parameters for depth (for 3D rendering)
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

	//Create openGL context
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

//Drawing function
void display(void)
{
	//Function declarations
	void I1(void);
	void N(void);
	void D(void);
	void I2(void);
	void A(void);
	void tricolor(void);

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (timer >= 1)
	{
		glTranslatef(I1_movement, 0.0f, -2.5f);
		//glTranslatef(-0.8f, 0.0f, -2.5f);	//actual position of I
		I1();
		if (I1_movement >= -0.8f)
			timer = 2;
	}

	if (timer >= 2)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(-0.5f, N_movement, -2.5f);
		//glTranslatef(-0.5f, 0.0f, -2.5f);	//actual position of N
		N();
		if (N_movement <= 0.0f)
			timer = 3;
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (timer >= 3)
	{
		glTranslatef(-0.2f, 0.0f, -2.5f);
		//glTranslatef(-0.2f, 0.0f, -2.5f);	//actual position of D
		D();
		if (Red >= 1.0f)	//To initiate next animation of I after fade in effect sets in
			timer = 4;
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (timer >= 4)
	{
		//glTranslatef(0.1f, 0.0f, -2.5f);	//Actual position of I2
		glTranslatef(0.1f, I2_movement, -2.5f);
		I2();
		if (I2_movement >= 0.0f)
			timer = 5;
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (timer >= 5)
	{
		//glTranslatef(0.4f, 0.0f, -2.5f);	Actual position of A
		glTranslatef(A_movement, 0.0f, -2.5f);
		A();
		if (A_movement <= 0.4f)
			timer = 6;
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (timer >= 6)
	{
		glTranslatef(tricolor_movement, 0.0f, -2.5f);
		tricolor();

	}

	SwapBuffers(ghdc);
}

//Resize the window
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

//Uninitialize all the resources on exit
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

//Draw alphabet I
void I1(void)
{
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	
	glColor3f(1.0f, 0.5f, 0.0f);	//Orange color
	glVertex3f(-0.9f, 0.9f, 0.0f);

	glColor3f(0.0f, 1.0f, 0.0f);	//Green color
	glVertex3f(-0.9f, -0.9f, 0.0f);

	glEnd();
}

//Draw alphabet N
void N(void)
{
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	
	//Left line of N
	glColor3f(1.0f, 0.5f, 0.0f);	//Orange color
	glVertex3f(-0.8f, 0.9f, 0.0f);

	glColor3f(0.0f, 1.0f, 0.0f);	//Green color
	glVertex3f(-0.8f, -0.9f, 0.0f);

	//Right line of N
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(-0.4f, 0.9f, 0.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.4f, -0.9f, 0.0f);

	//Diagonal line of N
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(-0.8f, 0.9f, 0.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-0.4f, -0.9f, 0.0f);

	glEnd();
}

//Draw alphabet D
void D(void)
{	
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	
	//Top line of D
	//glColor3f(1.0f, 0.5f, 0.0f);	//Original orange
	glColor3f(Red, Green2, 0.0f);	//Orange color - fading effect

	glVertex3f(-0.3f, 0.9f, 0.0f);
	glVertex3f(0.1f, 0.9f, 0.0f);

	//Bottom line of D
	//glColor3f(0.0f, 1.0f, 0.0f);	//Original green
	glColor3f(0.0f, Green1, 0.0f);	//Green color - fading effect
	glVertex3f(-0.3f, -0.9f, 0.0f);
	glVertex3f(0.1f, -0.9f, 0.0f);

	//Left line of D
	//glColor3f(1.0f, 0.5f, 0.0f);	//Original orange
	glColor3f(Red, Green2, 0.0f);
	glVertex3f(-0.28f, 0.9f, 0.0f);

	//glColor3f(0.0f, 1.0f, 0.0f);	//Original Green
	glColor3f(0.0f, Green1, 0.0f);
	glVertex3f(-0.28f, -0.9f, 0.0f);
			
	//Right line of D
	//glColor3f(1.0f, 0.5f, 0.0f);	//Original Orange
	glColor3f(Red, Green2, 0.0f);
	glVertex3f(0.1f, 0.905f, 0.0f);
			
	//glColor3f(0.0f, 1.0f, 0.0f);	//Original Green
	glColor3f(0.0f, Green1, 0.0f);
	glVertex3f(0.1f, -0.905f, 0.0f);

	glEnd();
	
}

//Draw alphabet I
void I2(void)
{
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(0.2f, 0.9f, 0.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.2f, -0.9f, 0.0f);

	glEnd();
}

//Draw alphabet A
void A(void)
{
	glLineWidth(5.0f);
	glBegin(GL_LINES);

	//Left diagonal line of A
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(0.75f, 0.9f, 0.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.3f, -0.9f, 0.0f);

	//Right diagonal line of A
	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex3f(0.75f, 0.9f, 0.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.2f, -0.9f, 0.0f);

	glEnd();

}

//Tricolor drawing function
void tricolor(void)
{
	glLineWidth(5.0f);
	glBegin(GL_LINES);

	if (tricolor_movement >= 1.375f)
	{
		
		//Center Line of A
		//Orange line
		glColor3f(1.0f, 0.5f, 0.0f);		//Orange color
		//glVertex3f(0.528f, 0.02f, 0.0f);	//Original co-ordinates
		//glVertex3f(0.974, 0.02f, 0.0f);
		glVertex3f(orange_left, 0.02f, 0.0f);
		glVertex3f(0.0f, 0.02f, 0.0f);

		//White line
		glColor3f(1.0f, 1.0f, 1.0f);		//White
		//glVertex3f(0.52f, 0.0f, 0.0f);	//Original co-ordinates
		//glVertex3f(0.98f, 0.0f, 0.0f);
		glVertex3f(white_left, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
			
		//Green line
		glColor3f(0.0f, 1.0f, 0.0f);		//Green color
		//glVertex3f(0.513f, -0.02f, 0.0f);	//Original Co-ordinates
		//glVertex3f(0.985f, -0.02f, 0.0f);
		glVertex3f(green_left, -0.02f, 0.0f);
		glVertex3f(0.0f, -0.02f, 0.0f);
	}
	else
	{

		//Center Line of A
		//Orange line
		glColor3f(1.0f, 0.5f, 0.0f);		//Orange color
		//glVertex3f(0.528f, 0.02f, 0.0f);	//Original co-ordinates
		//glVertex3f(0.974, 0.02f, 0.0f);
		glVertex3f(-3.5f, 0.02f, 0.0f);
		glVertex3f(0.0f, 0.02f, 0.0f);

		//White line
		glColor3f(1.0f, 1.0f, 1.0f);		//White
		//glVertex3f(0.52f, 0.0f, 0.0f);	//Original co-ordinates
		//glVertex3f(0.98f, 0.0f, 0.0f);
		glVertex3f(-3.5f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);

		//Green line
		glColor3f(0.0f, 1.0f, 0.0f);		//Green color
		//glVertex3f(0.513f, -0.02f, 0.0f);	//Original Co-ordinates
		//glVertex3f(0.985f, -0.02f, 0.0f);
		glVertex3f(-3.5f, -0.02f, 0.0f);
		glVertex3f(0.0f, -0.02f, 0.0f);
	}
		glEnd();
}

//Update the values so that it is rendered in the game loop
void Update(void)
{

	if (timer >= 1 && I1_movement <= -0.8f)
		I1_movement = I1_movement + 0.00005f;		//For animation of I1
	if (timer >= 2 && N_movement >= 0.0f)
		N_movement = N_movement - 0.00005f;		//For animation of N
	if (timer >= 3)						//For animation of D
	{
		if (Red <= 1.0f)		//For Orange fade in
			Red = Red + 0.00005f;
		if (Green1 <= 1.0f)		//For Green fade in
			Green1 = Green1 + 0.00005f;
		if (Green2 <= 0.5f)		//For Orange fade in
			Green2 = Green2 + 0.00005f;
	}
	if (timer >= 4 && I2_movement <= 0.0f)
		I2_movement = I2_movement + 0.00005f;		//For animation of I2
	if (timer >= 5 && A_movement >= 0.4f)
		A_movement = A_movement - 0.00005f;		//For animation of A
	if (timer >= 6 && tricolor_movement <= 1.375f)
		tricolor_movement = tricolor_movement + 0.00009f;
	if(tricolor_movement >= 1.375f)
	{
		if (orange_left <= -0.45f)
			orange_left = orange_left + 0.00009f;
		if (white_left <= -0.455f)
			white_left = white_left + 0.00009f;
		if (green_left <= -0.46f)
			green_left = green_left + 0.00009f;
	}
}	