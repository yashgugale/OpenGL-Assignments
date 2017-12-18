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

static int angle_x_Light = 0;
static int angle_y_Light = 0;
static int angle_z_Light = 0;

int x_rotation = 0;
int y_rotation = 0;
int z_rotation = 0;

bool gbLighting = false;
GLUquadric *quadric = NULL;

//Lighting arrays:
//LIGHT0: GLOBAL WHITE LIGHT
GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };	//White colored light
GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_position[] = { 0.0f, 0.0f, 1.0f, 0.0f };	//Position of light

GLfloat light_model_ambient[] = { 0.2f, 0.2f, 0.2f, 0.0f };
GLfloat light_model_local_viewer[] = { 0.0f, 0.0f, 0.0f };

//MATERIALS OF THE SPHERES:
//1. EMERALD:
GLfloat s1_c1_material_ambient[] = { 0.0215f, 0.1745f, 0.0215f, 1.0f };
GLfloat s1_c1_material_diffuse[] = { 0.07568f, 0.61424f, 0.07568f, 1.0f };
GLfloat s1_c1_material_specular[] = { 0.633f, 0.727811f, 0.633f, 1.0f };
GLfloat s1_c1_material_shininess[] = { 0.6f * 128.0f };

//2. JADE:
GLfloat s2_c1_material_ambient[] = { 0.135f, 0.2225f, 0.1575f, 1.0f };
GLfloat s2_c1_material_diffuse[] = { 0.54f, 0.89f, 0.63f, 1.0f };
GLfloat s2_c1_material_specular[] = { 0.316228, 0.316228, 0.316228, 1.0f };
GLfloat s2_c1_material_shininess[] = { 0.1f * 128.0f };

//3. OBSIDIAN:
GLfloat s3_c1_material_ambient[] = { 0.05375f, 0.05f, 0.06625f, 1.0f };
GLfloat s3_c1_material_diffuse[] = { 0.18275f, 0.17f, 0.22525f, 1.0f };
GLfloat s3_c1_material_specular[] = { 0.332741f, 0.328634f, 0.346435f, 1.0f };
GLfloat s3_c1_material_shininess[] = { 0.3f * 128.0f };

//4. PEARL:
GLfloat s4_c1_material_ambient[] = { 0.25f, 0.20725f, 0.20725f, 1.0f };
GLfloat s4_c1_material_diffuse[] = { 1.0f, 0.829f, 0.829f, 1.0f };
GLfloat s4_c1_material_specular[] = { 0.296648f, 0.296648f, 0.296648f, 1.0f };
GLfloat s4_c1_material_shininess[] = { 0.088f * 128.0f };

//5. RUBY:
GLfloat s5_c1_material_ambient[] = { 0.1745f, 0.01175f, 0.01175f, 1.0f };
GLfloat s5_c1_material_diffuse[] = { 0.61424f, 0.04136f, 0.04136f, 1.0f };
GLfloat s5_c1_material_specular[] = { 0.727811f, 0.626959f, 0.626959f, 1.0f };
GLfloat s5_c1_material_shininess[] = { 0.6f * 128.0f };

//6. TURQUOISE:
GLfloat s6_c1_material_ambient[] = { 0.1f, 0.18725f, 0.1745f, 1.0f };
GLfloat s6_c1_material_diffuse[] = { 0.396f, 0.74151f, 0.69102f, 1.0f };
GLfloat s6_c1_material_specular[] = { 0.297254f, 0.30829f, 0.306678f, 1.0f };
GLfloat s6_c1_material_shininess[] = { 0.1f * 128.0f };

//7. BRASS:
GLfloat s1_c2_material_ambient[] = { 0.329412f, 0.223529f, 0.027451f, 1.0f };
GLfloat s1_c2_material_diffuse[] = { 0.780392f, 0.568627f, 0.113725f, 1.0f };
GLfloat s1_c2_material_specular[] = { 0.992157f, 0.941176f, 0.807843f, 1.0f };
GLfloat s1_c2_material_shininess[] = { 0.21794872f * 128.0f };

//8. BRONZE:
GLfloat s2_c2_material_ambient[] = { 0.2125f, 0.1275f, 0.054f, 1.0f };
GLfloat s2_c2_material_diffuse[] = { 0.714f, 0.4284f, 0.18144f, 1.0f };
GLfloat s2_c2_material_specular[] = { 0.393548f, 0.271906f, 0.166721f, 1.0f };
GLfloat s2_c2_material_shininess[] = { 0.2f * 128.0f };

//9. CHROME:
GLfloat s3_c2_material_ambient[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat s3_c2_material_diffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };
GLfloat s3_c2_material_specular[] = { 0.774597f, 0.774597f, 0.774597f, 1.0f };
GLfloat s3_c2_material_shininess[] = { 0.6f * 128.0f };

//10. COPPER:
GLfloat s4_c2_material_ambient[] = { 0.19125f, 0.0735f, 0.0225f, 1.0f };
GLfloat s4_c2_material_diffuse[] = { 0.7038f, 0.27048f, 0.0828f, 1.0f };
GLfloat s4_c2_material_specular[] = { 0.256777f, 0.137622f, 0.086014f, 1.0f };
GLfloat s4_c2_material_shininess[] = { 0.1f * 128.0f };

//11. GOLD:
GLfloat s5_c2_material_ambient[] = { 0.24725f, 0.1995f, 0.0745f, 1.0f };
GLfloat s5_c2_material_diffuse[] = { 0.75164f, 0.60648f, 0.22648f, 1.0f };
GLfloat s5_c2_material_specular[] = { 0.628281f, 0.555802f, 0.366065f, 1.0f };
GLfloat s5_c2_material_shininess[] = { 0.4f * 128.0f };

//12. SILVER:
GLfloat s6_c2_material_ambient[] = { 0.19225f, 0.19225f, 0.19225f, 1.0f };
GLfloat s6_c2_material_diffuse[] = { 0.50754f, 0.50754f, 0.50754f, 1.0f };
GLfloat s6_c2_material_specular[] = { 0.508273f, 0.508273f, 0.508273f, 1.0f };
GLfloat s6_c2_material_shininess[] = { 0.4f * 128.0f };

//13. BLACK:
GLfloat s1_c3_material_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat s1_c3_material_diffuse[] = { 0.01f, 0.01f, 0.01f, 1.0f };
GLfloat s1_c3_material_specular[] = { 0.50f, 0.50f, 0.50f, 1.0f };
GLfloat s1_c3_material_shininess[] = { 0.25f * 128.0f };

//14. CYAN:
GLfloat s2_c3_material_ambient[] = { 0.0f, 0.1f, 0.06f, 1.0f };
GLfloat s2_c3_material_diffuse[] = { 0.0f, 0.50980392f, 0.50980392f, 1.0f };
GLfloat s2_c3_material_specular[] = { 0.50196078f, 0.50196078f, 0.50196078f, 1.0f };
GLfloat s2_c3_material_shininess[] = { 0.25f * 128.0f };

//15. GREEN:
GLfloat s3_c3_material_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat s3_c3_material_diffuse[] = { 0.1f, 0.35f, 0.1f, 1.0f };
GLfloat s3_c3_material_specular[] = { 0.45f, 0.55f, 0.45f, 1.0f };
GLfloat s3_c3_material_shininess[] = { 0.25f * 128.0f };

//16. RED:
GLfloat s4_c3_material_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat s4_c3_material_diffuse[] = { 0.5f, 0.0f, 0.0f, 1.0f };
GLfloat s4_c3_material_specular[] = { 0.7f, 0.6f, 0.6f, 1.0f };
GLfloat s4_c3_material_shininess[] = { 0.25f * 128.0f };

//17. WHITE:
GLfloat s5_c3_material_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat s5_c3_material_diffuse[] = { 0.55f, 0.55f, 0.55f, 1.0f };
GLfloat s5_c3_material_specular[] = { 0.70f, 0.70f, 0.70f, 1.0f };
GLfloat s5_c3_material_shininess[] = { 0.25f * 128.0f };

//18. YELLOW PLASTIC:
GLfloat s6_c3_material_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat s6_c3_material_diffuse[] = { 0.5f, 0.5f, 0.0f, 1.0f };
GLfloat s6_c3_material_specular[] = { 0.60f, 0.60f, 0.50f, 1.0f };
GLfloat s6_c3_material_shininess[] = { 0.25f * 128.0f };

//19. BLACK:
GLfloat s1_c4_material_ambient[] = { 0.02f, 0.02f, 0.02f, 1.0f };
GLfloat s1_c4_material_diffuse[] = { 0.01f, 0.01f, 0.01f, 1.0f };
GLfloat s1_c4_material_specular[] = { 0.4f, 0.4f, 0.4f, 1.0f };
GLfloat s1_c4_material_shininess[] = { 0.078125f * 128.0f };

//20. CYAN:
GLfloat s2_c4_material_ambient[] = { 0.0f, 0.05f, 0.05f, 1.0f };
GLfloat s2_c4_material_diffuse[] = { 0.4f, 0.5f, 0.5f, 1.0f };
GLfloat s2_c4_material_specular[] = { 0.04f, 0.7f, 0.7f, 1.0f };
GLfloat s2_c4_material_shininess[] = { 0.078125f * 128.0f };

//21. GREEN:
GLfloat s3_c4_material_ambient[] = { 0.0f, 0.05f, 0.0f, 1.0f };
GLfloat s3_c4_material_diffuse[] = { 0.4f, 0.5f, 0.4f, 1.0f };
GLfloat s3_c4_material_specular[] = { 0.04f, 0.7f, 0.04f, 1.0f };
GLfloat s3_c4_material_shininess[] = { 0.078125f * 128.0f };

//22. RED:
GLfloat s4_c4_material_ambient[] = { 0.05f, 0.0f, 0.0f, 1.0f };
GLfloat s4_c4_material_diffuse[] = { 0.5f, 0.4f, 0.4f, 1.0f };
GLfloat s4_c4_material_specular[] = { 0.7f, 0.04f, 0.04f, 1.0f };
GLfloat s4_c4_material_shininess[] = { 0.078125f * 128.0f };

//23. WHITE:
GLfloat s5_c4_material_ambient[] = { 0.05f, 0.05f, 0.05f, 1.0f };
GLfloat s5_c4_material_diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat s5_c4_material_specular[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat s5_c4_material_shininess[] = { 0.078125f * 128.0f };

//24. YELLOW RUBBER
GLfloat s6_c4_material_ambient[] = { 0.05f, 0.05f, 0.0f, 1.0f };
GLfloat s6_c4_material_diffuse[] = { 0.5f, 0.5f, 0.4f, 1.0f };
GLfloat s6_c4_material_specular[] = { 0.7f, 0.7f, 0.04f, 1.0f };
GLfloat s6_c4_material_shininess[] = { 0.078125f * 128.0f };

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
		TEXT("OpenGL Materials"),
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
	//void display(void);
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
		case 0x58:	//For 'x' or 'X'
			x_rotation = 1;
			y_rotation = 0;
			z_rotation = 0;
			break;
		case 0x59:	//For 'y' or 'Y'
			x_rotation = 0;
			y_rotation = 1;
			z_rotation = 0;
			break;
		case 0x5A:	//For 'z' or 'Z'
			x_rotation = 0;
			y_rotation = 0;
			z_rotation = 1;
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

	glClearColor(0.25f, 0.25f, 0.25f, 0.0f);	//Dark gray background

	glClearDepth(1.0f);			//clear depth buffer
	glEnable(GL_DEPTH_TEST);	//enable the depth
	glDepthFunc(GL_LEQUAL);		//less than or equal to 1.0f in far. Uses ray tracing algorithm

	//Optional Calls
	glShadeModel(GL_SMOOTH);							//to remove aliasing
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	//to remove distortion

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_model_ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, light_model_local_viewer);

	//Lighting calls
	//LIGHT0:
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	//Enable LIGHT0
	glEnable(GL_LIGHT0);

	//Create a quadric for the sphere
	quadric = gluNewQuadric();

	resize(WIN_WIDTH, WIN_HEIGHT);
}

void display(void)
{

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	gluLookAt(0.0f, 0.0f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	if (x_rotation == 1)
	{
		glPushMatrix();
		glRotatef((GLfloat)angle_x_Light, 1.0f, 0.0f, 0.0f);		//Light rotating about X-axis
		light_position[1] = angle_x_Light;
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glPopMatrix();
	}
	else if (y_rotation == 1)
	{
		glPushMatrix();
		glRotatef((GLfloat)angle_y_Light, 0.0f, 1.0f, 0.0f);		//Light rotating about Y-axis
		light_position[0] = angle_y_Light;
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glPopMatrix();
	}
	else if (z_rotation == 1)
	{
		glPushMatrix();
		glRotatef((GLfloat)angle_z_Light, 0.0f, 0.0f, 1.0f);		//Light rotating about Z-axis
		light_position[0] = angle_z_Light;
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glPopMatrix();
	}
	
	//COLUMN - 1
	//1st sphere on 1st column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s1_c1_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s1_c1_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s1_c1_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s1_c1_material_shininess);

	glPushMatrix();
	glTranslatef(-4.0f, 6.5f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();

	//2nd sphere on 1st column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s2_c1_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s2_c1_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s2_c1_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s2_c1_material_shininess);

	glPushMatrix();
	glTranslatef(-4.0f, 4.0f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();

	//3rd sphere on 1st column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s3_c1_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s3_c1_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s3_c1_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s3_c1_material_shininess);

	glPushMatrix();
	glTranslatef(-4.0f, 1.5f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();

	//4th sphere on 1st column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s4_c1_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s4_c1_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s4_c1_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s4_c1_material_shininess);

	glPushMatrix();
	glTranslatef(-4.0f, -1.0f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();

	//5th sphere on 1st column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s5_c1_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s5_c1_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s5_c1_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s5_c1_material_shininess);

	glPushMatrix();
	glTranslatef(-4.0f, -3.5f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();

	//6th sphere on 1st column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s6_c1_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s6_c1_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s6_c1_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s6_c1_material_shininess);

	glPushMatrix();
	glTranslatef(-4.0f, -6.0f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();

	//COLUMN - 2
	//1st sphere on 2nd column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s1_c2_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s1_c2_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s1_c2_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s1_c2_material_shininess);

	glPushMatrix();
	glTranslatef(-1.5f, 6.5f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();

	//2nd sphere on 2nd column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s2_c2_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s2_c2_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s2_c2_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s2_c2_material_shininess);

	glPushMatrix();
	glTranslatef(-1.5f, 4.0f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();

	//3rd sphere on 2nd column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s3_c2_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s3_c2_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s3_c2_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s3_c2_material_shininess);

	glPushMatrix();
	glTranslatef(-1.5f, 1.5f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();

	//4th sphere on 2nd column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s4_c2_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s4_c2_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s4_c2_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s4_c2_material_shininess);

	glPushMatrix();
	glTranslatef(-1.5f, -1.0f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();

	//5th sphere on 2nd column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s5_c2_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s5_c2_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s5_c2_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s5_c2_material_shininess);

	glPushMatrix();
	glTranslatef(-1.5f, -3.5f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();

	//6th sphere on 2nd column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s6_c2_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s6_c2_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s6_c2_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s6_c2_material_shininess);

	glPushMatrix();
	glTranslatef(-1.5f, -6.0f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();

	//COLUMN - 3
	//1st sphere on 3rd column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s1_c3_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s1_c3_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s1_c3_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s1_c3_material_shininess);

	glPushMatrix();
	glTranslatef(1.0f, 6.5f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();

	//2nd sphere on 3rd column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s2_c3_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s2_c3_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s2_c3_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s2_c3_material_shininess);

	glPushMatrix();
	glTranslatef(1.0f, 4.0f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();

	//3rd sphere on 3rd column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s3_c3_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s3_c3_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s3_c3_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s3_c3_material_shininess);

	glPushMatrix();
	glTranslatef(1.0f, 1.5f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();

	//4th sphere on 3rd column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s4_c3_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s4_c3_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s4_c3_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s4_c3_material_shininess);

	glPushMatrix();
	glTranslatef(1.0f, -1.0f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();

	//5th sphere on 3rd column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s5_c3_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s5_c3_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s5_c3_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s5_c3_material_shininess);

	glPushMatrix();
	glTranslatef(1.0f, -3.5f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();

	//6th sphere on 3rd column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s6_c3_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s6_c3_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s6_c3_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s6_c3_material_shininess);

	glPushMatrix();
	glTranslatef(1.0f, -6.0f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();

	//COLUMN - 4
	//1st sphere on 4th column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s1_c4_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s1_c4_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s1_c4_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s1_c4_material_shininess);

	glPushMatrix();
	glTranslatef(3.5f, 6.5f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();

	//2nd sphere on 4th column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s2_c4_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s2_c4_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s2_c4_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s2_c4_material_shininess);

	glPushMatrix();
	glTranslatef(3.5f, 4.0f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();

	//3rd sphere on 4th column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s3_c4_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s3_c4_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s3_c4_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s3_c4_material_shininess);

	glPushMatrix();
	glTranslatef(3.5f, 1.5f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();

	//4th sphere on 4th column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s4_c4_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s4_c4_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s4_c4_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s4_c4_material_shininess);

	glPushMatrix();
	glTranslatef(3.5f, -1.0f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();

	//5th sphere on 4th column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s5_c4_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s5_c4_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s5_c4_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s5_c4_material_shininess);

	glPushMatrix();
	glTranslatef(3.5f, -3.5f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();

	//6th sphere on 4th column
	//Material:
	glMaterialfv(GL_FRONT, GL_AMBIENT, s6_c4_material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, s6_c4_material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s6_c4_material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, s6_c4_material_shininess);

	glPushMatrix();
	glTranslatef(3.5f, -6.0f, -20.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	gluSphere(quadric, 1.0f, 100, 100);
	glPopMatrix();


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

	gluDeleteQuadric(quadric);	//Delete the quadric

	wglMakeCurrent(NULL, NULL);

	wglDeleteContext(ghrc);
	ghrc = NULL;

	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	DestroyWindow(ghwnd);
	ghwnd = NULL;
}


void update(void)
{

	//code	
	//Rotate the light about x-axis
	angle_x_Light = (angle_x_Light - 1) % 360;

	//Rotate the light about y-axis
	angle_y_Light = (angle_y_Light - 1) % 360;

	//Rotate the light about z-axis
	angle_z_Light = (angle_z_Light - 1) % 360;

	
}
