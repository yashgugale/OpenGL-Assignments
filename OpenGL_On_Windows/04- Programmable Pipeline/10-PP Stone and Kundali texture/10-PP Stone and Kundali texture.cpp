#include <windows.h>
#include <stdio.h>		// For file I/O

#include <gl\glew.h>	// For GLSL extensions. It must be included before <gl\GL.h>
#include <gl\GL.h>

#include "vmath.h"			// Header file from Red Book (For maths) - (v-vermilion)
#include "MSOGLWindow.h"	// Header for texture objects

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

using namespace vmath;

// Properties of the vertex
enum
{
	YSG_ATTRIBUTE_POSITION = 0,
	YSG_ATTRIBUTE_COLOR,
	YSG_ATTRIBUTE_NORMAL,
	YSG_ATTRIBUTE_TEXTURE0
};

//Prototype Of WndProc() declared Globally
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global variable declarations
FILE *gpFile = NULL;		// Global pointer, so that log file can be opened anywhere

HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullscreen = false;

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

GLuint gVaoPyramid;
GLuint gVaoCube;

GLuint gVboPyramidPosition;
GLuint gVboPyramidTexture;

GLuint gVboCubePosition;
GLuint gVboCubeTexture;

GLuint gMVPUniform;
GLuint gTextureSamplerUniform;	// to catch uniform of sampler

GLuint gTextureStone;			// Stone texture object
GLuint gTextureKundali;			// Kundali texture object

mat4 gPerspectiveProjectionMatrix;

// Rotation Variables:
float anglePyramid = 0.0f;
float angleCube = 0.0f;

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
	TCHAR szClassName[] = TEXT("OpenGLPP");
	bool bDone = false;

	//code
	// Create log file for debugging
	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log File Can Not Be Created\nExitting ..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log File Is Successfully Opened.\n");
	}

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

	RegisterClassEx(&wndclass);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("OpenGL Programmable Pipeline : Stone and kundali texture"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Initialize
	initialize();

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
			// Update for rotation:
			update();
			// Drawing / rendering function
			display();
			if (gbActiveWindow == true)
			{
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

	// Variable declarations:
	static WORD xMouse = NULL;
	static WORD yMouse = NULL;

	//code
	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)		// If the window is active
			gbActiveWindow = true;
		else							// If non-zero, the window is inactive
			gbActiveWindow = false;
		break;
	case WM_ERASEBKGND:
		return(0);
	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			gbEscapeKeyIsPressed = true;
			break;
		case 0x46:
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
	case WM_CLOSE:
		uninitialize();
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
	void uninitialize(void);
	void resize(int, int);
	int LoadGLTextures(GLuint *, TCHAR[]);

	//variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	//code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

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

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == false)
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

	if (wglMakeCurrent(ghdc, ghrc) == false)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	// GLEW initializtion code for GLSL.
	// It must be here, ie, after creating OpenGL context but before using and OpenGL functions
	GLenum glew_error = glewInit();
	if (glew_error != GLEW_OK)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	/*
	GLint number_of_extensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &number_of_extensions);		// Returns the number of available extensions
	fprintf(gpFile, "\nPrinting the available extensions : \n");
	for (int i = 0; i < number_of_extensions; i++)
	{
		fprintf(gpFile, "%d. %s\n", i+1, (const char *)glGetStringi(GL_EXTENSIONS, i));
	} 
	fprintf(gpFile, "\n");
	*/

	// *** VERTEX SHADER ***
	// 1. Create the shader (object is created and assigned)
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	// 2. Provide the source code to shader
	// '\' indicates a single string and not 3 separate strings. Source code as array of characters.
	// Instead of keeping inside a file (eg vertexshader.vsh), we keep it as a string inside our source file
	
	// GLSL version number 1.3 (1.3 * 100 = 130). default = 1.1
	// vPosition and u_mvp_matrix are user defined names for matrices for transformation
	const GLchar *vertexShaderSourceCode =
		"#version 450 core"	\
		"\n"	\
		"in vec4 vPosition;"	\
		"in vec2 vTexture0_Coord;"	\
		"out vec2 out_texture0_coord;"	\
		"uniform mat4 u_mvp_matrix;"	\
		"void main(void)"	\
		"{"	\
		"gl_Position = u_mvp_matrix * vPosition;"
		"out_texture0_coord = vTexture0_Coord;"	\
		"}";
	
	// 3. Pass the object as the shader source. glVertexShaderObject is our specialist
	// 1 - 1program, so only 1 string. NULL - Pass length of string if not NUL terminated
	// (const GLchar **) - Pass the address of the program by casting (as array is used)
	glShaderSource(gVertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);

	// 4. Compile the shader - Dynamic compilation by the driver
	glCompileShader(gVertexShaderObject);

	// ERROR CHECKING:
	GLint iInfoLogLength = 0;			// Length of the log created on failure to compile
	GLint iShaderCompiledStatus = 0;	// Stores shader compilation status
	char *szInfoLog = NULL;				// String to store the log

	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)	// Failure to compile
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;		// Stores number of characters written
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Vertex shader compilation log : %s\n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				uninitialize();
				exit(0);
			}
		}
	}

	// *** FRAGMENT SHADER ***
	// Create the shader
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	// Pass the source code to shader
	// FragColor is our name for the Fragment
	const GLchar *fragmentShaderSourceCode =
		"#version 450 core"	\
		"\n"	\
		"in vec2 out_texture0_coord;"
		"out vec4 FragColor;"	\
		"uniform sampler2D u_texture0_sampler;"
		"void main(void)"	\
		"{"	\
		"FragColor = texture(u_texture0_sampler, out_texture0_coord);"	\
		"}";

	glShaderSource(gFragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);

	// compiler the shader
	glCompileShader(gFragmentShaderObject);
	
	// Error checking for compilation done here
	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)	// Failure to compile
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;		// Stores number of characters written
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Fragment shader compilation log : %s\n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				uninitialize();
				exit(0);
			}
		}
	}
	// *** SHADER PROGRAM ***
	// Create
	gShaderProgramObject = glCreateProgram();		// It can link ALL the shaders. Hence, no parameter

	// Attach vertex shader to the shader program
	glAttachShader(gShaderProgramObject, gVertexShaderObject);

	// Attach fragment shader to the shader program
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);
	
	// Pre link binding of shader program object with vertex shader position attribute
	// This shader object is recognized by out YSG_ATTRIBUTE_POSITION
	glBindAttribLocation(gShaderProgramObject, YSG_ATTRIBUTE_POSITION, "vPosition");

	// Pre link binding of shader program object with fragment shader texture attribute
	// This shader object is recognized by out YSG_ATTRIBUTE_TEXTURE0
	glBindAttribLocation(gShaderProgramObject, YSG_ATTRIBUTE_TEXTURE0, "vTexture0_Coord");

	// Link the shader
	glLinkProgram(gShaderProgramObject);
	
	// Error checking for linking
	GLint iShaderProgramLinkStatus = 0;

	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)	// Failure to link
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;		// Stores number of characters written
				glGetProgramInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Program shader link log : %s\n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
				uninitialize();
				exit(0);
			}
		}
	}

	// Get MVP uniform locations:
	gMVPUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
	gTextureSamplerUniform = glGetUniformLocation(gShaderProgramObject, "u_texture0_sampler");

	// Vertices, Colors, Shader attributes, vbo, vao initializations:
	const GLfloat pyramidVertices[] = 
	{	// Perspective triangle (Front face)
		0.0f, 1.0f, 0.0f,		// Apex
		-1.0f, -1.0f, 1.0f,		// Left bottom
		1.0f, -1.0f, 1.0f,		// Right bottom
								// Perspective triangle (Right face)
		0.0f, 1.0f, 0.0f,		// Apex
		1.0f, -1.0f, 1.0f,		// Left bottom
		1.0f, -1.0f, -1.0f,		// Right bottom
								// Perspective triangle (Back face)
		0.0f, 1.0f, 0.0f,		// Apex
		1.0f, -1.0f, -1.0f,		// Left bottom
		-1.0f, -1.0f, -1.0f,	// Right bottom
								// Perspective triangle (Left face)
		0.0f, 1.0f, 0.0f,		// Apex
		-1.0f, -1.0f, -1.0f,	// Left bottom
		-1.0f, -1.0f, 1.0f		// Right bottom
	};

	const GLfloat pyramidTexCoords[] = 
	{							// Stone textured Pyramid
		0.5f, 1.0f, 
		0.0f, 0.0f, 
		1.0f, 0.0f, 

		0.5f, 1.0f, 
		1.0f, 0.0f, 
		0.0f, 0.0f, 

		0.5f, 1.0f, 
		1.0f, 0.0f, 
		0.0f, 0.0f, 

		0.5f, 1.0f, 
		0.0f, 0.0f, 
		1.0f, 0.0f, 
	};

	GLfloat cubeVertices[] = 
	{							// Perspective square (Top face)
		1.0f, 1.0f, -1.0f,		// Right top
		-1.0f, 1.0f, -1.0f, 	// Left top
		-1.0f, 1.0f, 1.0f,		// Left bottom
		1.0f, 1.0f, 1.0f,		// Right bottom
								// Perspective square (Bottom face)
		1.0f, -1.0f, -1.0f,		// Right top
		-1.0f, -1.0f, -1.0f, 	// Left top
		-1.0f, -1.0f, 1.0f,		// Left bottom
		1.0f, -1.0f, 1.0f,		// Right bottom
								// Perspective square (Front face)
		1.0f, 1.0f, 1.0f,		// Right top
		-1.0f, 1.0f, 1.0f,		// Left top
		-1.0f, -1.0f, 1.0f, 	// Left bottom
		1.0f, -1.0f, 1.0f,		// Right bottom
								// Perspective square (Back face)
		1.0f, 1.0f, -1.0f,		// Right top											
		-1.0f, 1.0f, -1.0f,		// Left top
		-1.0f, -1.0f, -1.0f, 	// Left bottom
		1.0f, -1.0f, -1.0f,		// Right bottom
								// Perspective square (Right face)
		1.0f, 1.0f, -1.0f,		// Right top											
		1.0f, 1.0f, 1.0f,		// Left top
		1.0f, -1.0f, 1.0f, 		// Left bottom
		1.0f, -1.0f, -1.0f,		// Right bottom
								// Perspective square (Left face)
		-1.0f, 1.0f, 1.0f,		// Right top																						
		-1.0f, 1.0f, -1.0f,		// Left top
		-1.0f, -1.0f, -1.0f, 	// Left bottom
		-1.0f, -1.0f, 1.0f		// Right bottom
	};

	// If above -1.0f Or +1.0f Values Make Cube Much Larger Than Pyramid,
	// then follow the code in following loop which will convert all 1s And -1s to -0.75 or +0.75
	for (int i = 0; i<72; i++)
	{
		if (cubeVertices[i]<0.0f)
			cubeVertices[i] = cubeVertices[i] + 0.25f;
		else if (cubeVertices[i]>0.0f)
			cubeVertices[i] = cubeVertices[i] - 0.25f;
		else
			cubeVertices[i] = cubeVertices[i];			// no change
	}

	const GLfloat cubeTexCoords[] = 
	{							// Kundali textured Cube
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
	};

	// A. BLOCK FOR PYRAMID:
	glGenVertexArrays(1, &gVaoPyramid);
	glBindVertexArray(gVaoPyramid);

	// A. BUFFER BLOCK FOR VERTICES:
	glGenBuffers(1, &gVboPyramidPosition);					// Buffer to store vertex position
	glBindBuffer(GL_ARRAY_BUFFER, gVboPyramidPosition);		// Find that named object in memory
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);	// Takes data from CPU to GPU

	glVertexAttribPointer(YSG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(YSG_ATTRIBUTE_POSITION);

	// Release the buffer for vertices:
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// B. BUFFER BLOCK FOR TEXTURE:
	glGenBuffers(1, &gVboPyramidTexture);					// Buffer to store vertex texture
	glBindBuffer(GL_ARRAY_BUFFER, gVboPyramidTexture);		// Find that named object in memory
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidTexCoords), pyramidTexCoords, GL_STATIC_DRAW);	// Takes data from CPU to GPU

	glVertexAttribPointer(YSG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);		// 2 for s and t of texture
	glEnableVertexAttribArray(YSG_ATTRIBUTE_TEXTURE0);

	// Release the buffer for colors:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);				// Unbind 


	// B. BLOCK FOR CUBE:
	glGenVertexArrays(1, &gVaoCube);
	glBindVertexArray(gVaoCube);

	// A. BUFFER BLOCK FOR VERTICES:
	glGenBuffers(1, &gVboCubePosition);						// Buffer to store vertex position
	glBindBuffer(GL_ARRAY_BUFFER, gVboCubePosition);		// Find that named object in memory
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);	// Takes data from CPU to GPU

	glVertexAttribPointer(YSG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(YSG_ATTRIBUTE_POSITION);

	// Release the buffer for vertices:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// B. BUFFER BLOCK FOR COLORS:
	glGenBuffers(1, &gVboCubeTexture);					// Buffer to store vertex colors
	glBindBuffer(GL_ARRAY_BUFFER, gVboCubeTexture);		// Find that named object in memory
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexCoords), cubeTexCoords, GL_STATIC_DRAW);	// Takes data from CPU to GPU

	glVertexAttribPointer(YSG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(YSG_ATTRIBUTE_TEXTURE0);
	
	// Release the buffer for colors:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindVertexArray(0);				// Unbind

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glEnable(GL_CULL_FACE);			

	// Load kundali and stone textures:
	LoadGLTextures(&gTextureKundali, MAKEINTRESOURCE(IDBITMAP_KUNDALI));
	LoadGLTextures(&gTextureStone, MAKEINTRESOURCE(IDBITMAP_STONE));

	// Enable texture mapping:
	glEnable(GL_TEXTURE_2D);

	// Set the background color to black:
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Set PerspectiveMatrix to identity matrix
	gPerspectiveProjectionMatrix = mat4::identity();

	// Warm up call. (Not required for Windows as resize is called before WM_PAINT
	resize(WIN_WIDTH, WIN_HEIGHT);
}

int LoadGLTextures(GLuint *texture, TCHAR imageResourceId[])
{
	//variable declarations
	HBITMAP hBitmap = NULL;
	BITMAP bmp;
	int iStatus = FALSE;

	//code
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceId, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	glGenTextures(1, texture);

	if (hBitmap)
	{
		iStatus = TRUE;
		GetObject(hBitmap, sizeof(bmp), &bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	// Set 1 rather than default 4 for better performance

		//Texture steps:
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		//Generate mipmapped texture
		glTexImage2D(GL_TEXTURE_2D,
					0,
					GL_RGB,
					bmp.bmWidth,
					bmp.bmHeight,
					0,
					GL_BGR,
					GL_UNSIGNED_BYTE,
					bmp.bmBits);

		// Create mipmaps for this texture for better image quality:
		glGenerateMipmap(GL_TEXTURE_2D);

		DeleteObject(hBitmap);
	}
	return(iStatus);

}

void display(void)
{
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Start to use the OpenGL program object
	glUseProgram(gShaderProgramObject);		// Compiled and linked program goes to the driver

	// OpenGL Drawing:

	// A. DRAW PYRAMID:
	// Set the modelview, rotation, scale and modelviewprojection matrices to identity 
	mat4 modelViewMatrix = mat4::identity();
	mat4 modelViewProjectionMatrix = mat4::identity();
	mat4 rotationMatrix = mat4::identity();
	mat4 scaleMatrix = mat4::identity();

	// Translate the modelViewMatrix along the z axis
	modelViewMatrix = translate(-1.5f, 0.0f, -6.0f);

	// Rotate the rotationMatrix:
	rotationMatrix = rotate(anglePyramid, 0.0f, 1.0f, 0.0f);

	// Multiply modelViewMatrix with the rotationMatrix:
	modelViewMatrix = modelViewMatrix * rotationMatrix;

	// Multiply the ModelView and Perspective Matrix to get modelviewprojection matrix
	modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix;	// Order is important

	// Pass above modelviewprojection matrix to the vertex shader in "u_mvp_matrix" shader variable
	// whose position we have already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	// BIND with pyramid Texture (3 steps):
	glActiveTexture(GL_TEXTURE0);					// 1. 0th texture (corresponds to YSG_ATTRIBUTE_TEXTURE0)
	glBindTexture(GL_TEXTURE_2D, gTextureStone);	// 2. bind to the texture
	glUniform1i(gTextureSamplerUniform, 0);			// 3. 0th sampler enabled (as we have only 1 texture sampler in frag shader

	// BIND vao
	glBindVertexArray(gVaoPyramid);

	// Draw either by glDrawTraingles() or glDrawArrays() or glDrawElements()
	glDrawArrays(GL_TRIANGLES, 0, 12);	// 3(x, y, z) vertices in the pyramidVertices array

	// Unbind vao
	glBindVertexArray(0);

	// A. CUBE:
	// Set the modelview, rotation, scale and modelviewprojection matrices to identity 
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	rotationMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	// Scale the scaleMatrix to appropriate size:
	scaleMatrix = scale(0.75f, 0.75f, 0.75f);

	// Translate the modelViewMatrix along the z axis
	modelViewMatrix = translate(1.5f, 0.0f, -6.0f);

	// Multiply scaleMatrix and modelViewMatrix:
	modelViewMatrix = modelViewMatrix * scaleMatrix;

	// Rotate the rotationMatrix:
	rotationMatrix = rotate(angleCube, 1.0f, 0.0f, 0.0f);	// Rotate about x-axis
	// Multiply modelViewMatrix with the rotationMatrix:
	modelViewMatrix = modelViewMatrix * rotationMatrix;
	
	rotationMatrix = rotate(angleCube, 0.0f, 1.0f, 0.0f);	// Rotate about y-axis
	// Multiply modelViewMatrix with the rotationMatrix:
	modelViewMatrix = modelViewMatrix * rotationMatrix;

	rotationMatrix = rotate(angleCube, 0.0f, 0.0f, 1.0f);	// Rotate about z-axis
	// Multiply modelViewMatrix with the rotationMatrix:
	modelViewMatrix = modelViewMatrix * rotationMatrix;

	// Multiply the ModelView and Perspective Matrix to get modelviewprojection matrix
	modelViewProjectionMatrix = gPerspectiveProjectionMatrix * modelViewMatrix;	// Order is important

	// Pass above modelviewprojection matrix to the vertex shader in "u_mvp_matrix" shader variable
	// whose position we have already calculated in initWithFrame() by using glGetUniformLocation()
	glUniformMatrix4fv(gMVPUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	// BIND with cube Texture (3 steps):
	glActiveTexture(GL_TEXTURE0);					// 1. 0th texture (corresponds to YSG_ATTRIBUTE_TEXTURE0)
	glBindTexture(GL_TEXTURE_2D, gTextureKundali);	// 2. bind to the texture
	glUniform1i(gTextureSamplerUniform, 0);			// 3. 0th sampler enabled (as we have only 1 texture sampler in frag shader

	// BIND vao
	glBindVertexArray(gVaoCube);

	// Draw either by glDrawTraingles() or glDrawArrays() or glDrawElements()
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);	// 3(x, y, z) vertices in the cubeVertices array
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);	// 3(x, y, z) vertices in the cubeVertices array
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);	// 3(x, y, z) vertices in the cubeVertices array
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);	// 3(x, y, z) vertices in the cubeVertices array
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);	// 3(x, y, z) vertices in the cubeVertices array
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);	// 3(x, y, z) vertices in the cubeVertices array

	// Unbind vao
	glBindVertexArray(0);
	// Stop using the OpenGL program object
	glUseProgram(0);

	SwapBuffers(ghdc);
}

// Update function:
void update()
{
	anglePyramid = anglePyramid + 0.1f;
	if (anglePyramid >= 360.0f)
		anglePyramid = 0.0f;

	angleCube = angleCube - 0.1f;
	if (angleCube <= -360.0f)
		angleCube = 0.0f;
}

void resize(int width, int height)
{
	//code
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	// glPerspective(FOV, aspect ratio, near, far)
	gPerspectiveProjectionMatrix = perspective(45.0f, ((GLfloat)width / (GLfloat)height), 0.1f, 100.0f);
}

void uninitialize(void)
{
	//code
	if (gbFullscreen == true)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);

		ShowCursor(TRUE);

	}

	// Destroy triangle vao
	if (gVaoPyramid)
	{
		glDeleteVertexArrays(1, &gVaoPyramid);
		gVaoPyramid = 0;
	}

	// Destroy square vao 
	if (gVaoCube)
	{
		glDeleteVertexArrays(1, &gVaoCube);
		gVaoCube = 0;
	}

	// Destroy vbo for Pyramid Position:
	if (gVboPyramidPosition)
	{
		glDeleteBuffers(1, &gVboPyramidPosition);
		gVboPyramidPosition = 0;
	}

	// Destroy vbo for Pyramid Texture:
	if (gVboPyramidTexture)
	{
		glDeleteBuffers(1, &gVboPyramidTexture);
		gVboPyramidTexture = 0;

	}

	// Delete the stone texture:
	if (gTextureStone)
	{
		glDeleteTextures(1, &gTextureStone);
		gTextureStone = 0;
	}

	// Destroy vbo for Cube Position:
	if (gVboCubePosition)
	{
		glDeleteBuffers(1, &gVboCubePosition);
		gVboCubePosition = 0;
	}

	// Destroy vbo for Cube Texture:
	if (gVboCubeTexture)
	{
		glDeleteBuffers(1, &gVboCubeTexture);
		gVboCubeTexture = 0;
	}

	// Delete the kundali texture:
	if (gTextureKundali)
	{
		glDeleteTextures(1, &gTextureKundali);
		gTextureKundali = 0;
	}

	// Detach the shaders first before deleting
	// Detach vertex shader from shader program object
	glDetachShader(gShaderProgramObject,  gVertexShaderObject);
	// Detach fragment shader from shader program object
	glDetachShader(gShaderProgramObject, gFragmentShaderObject);

	// Delete vertex shader object
	glDeleteShader(gVertexShaderObject);
	gVertexShaderObject = 0;
	// Delete fragment shader object
	glDeleteShader(gFragmentShaderObject);
	gFragmentShaderObject = 0;

	// Delete shader program object. It has no objects attached
	glDeleteProgram(gShaderProgramObject);
	gShaderProgramObject = 0;

	// Unlink shader program
	glUseProgram(0);		// Stray call
	
	wglMakeCurrent(NULL, NULL);

	wglDeleteContext(ghrc);
	ghrc = NULL;

	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	if (gpFile)			// Closing the log file here
	{
		fprintf(gpFile, "Log File Is Successfully Closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
