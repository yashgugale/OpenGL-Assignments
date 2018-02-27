#include <windows.h>
#include <stdio.h>		// For file I/O

#include <gl\glew.h>	// For GLSL extensions. It must be included before <gl\GL.h>
#include <gl\GL.h>

#include "vmath.h"		// Header file from Red Book (For maths) - (v-vermilion)
#include "Sphere.h"		// For the sphere object

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"Sphere.lib")

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

// Key press:
bool isAKeyPressed = false;
bool isLKeyPressed = false;

GLuint gVertexShaderObject;
GLuint gFragmentShaderObject;
GLuint gShaderProgramObject;

// Sphere variables:
float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

GLuint gNumVertices;
GLuint gNumElements;

// Sphere Vao and Vbo:
GLuint gVaoSphere;
GLuint gVboSpherePosition;
GLuint gVboSphereNormal;
GLuint gVboSphereElement;

// Uniforms:
GLuint gModelMatrixUniform, gViewMatrixUniform, gProjectionMatrixUniform;

// Light uniforms:
GLuint gLaUniform;				// Uniform for ambient component of light
GLuint gLdUniform;				// Uniform for diffuse component of light
GLuint gLsUniform;				// Uniform for specular component of light
GLuint gLightPositionUniform;	// Uniform for light position

// Matrial uniforms:
GLuint gKaUniform;				// Uniform for ambient component of material
GLuint gKdUniform;				// Uniform for diffuse component of material
GLuint gKsUniform;				// Uniform for specular component of material
GLuint gMaterialShininessUniform;	// Uniform for shininess component of material

GLuint gLKeyPressedUniform;

mat4 gPerspectiveProjectionMatrix;

// Animate and Light variables:
bool gbAnimate;
bool gbLight;

GLfloat lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightPosition[] = { 100.0f,100.0f,100.0f,1.0f };

GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialShininess = 50.0f;

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
		TEXT("OpenGL Programmable Pipeline : Lights on Sphere (Per Vertex lighting)"),
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
			//if(gbAnimate == true)
				//update();
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
		/*
		case 0x41: // for 'A' or 'a'
			if (isAKeyPressed == false)
			{
				gbAnimate = true;
				isAKeyPressed = true;
			}
			else
			{
				gbAnimate = false;
				isAKeyPressed = false;
			}
			break;
		*/
		case 0x4C: // for 'L' or 'l'
			if (isLKeyPressed == false)
			{
				gbLight = true;
				isLKeyPressed = true;
			}
			else
			{
				gbLight = false;
				isLKeyPressed = false;
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

	// PER VERTEX LIGHTING:
	const GLchar *vertexShaderSourceCode =
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_model_matrix;" \
		"uniform mat4 u_view_matrix;" \
		"uniform mat4 u_projection_matrix;" \
		"uniform int u_lighting_enabled;" \
		"uniform vec3 u_La;" \
		"uniform vec3 u_Ld;" \
		"uniform vec3 u_Ls;" \
		"uniform vec4 u_light_position;" \
		"uniform vec3 u_Ka;" \
		"uniform vec3 u_Kd;" \
		"uniform vec3 u_Ks;" \
		"uniform float u_material_shininess;" \
		"out vec3 phong_ads_color;" \
		"void main(void)" \
		"{" \
		"if(u_lighting_enabled==1)" \
		"{" \
		"vec4 eye_coordinates=u_view_matrix * u_model_matrix * vPosition;" \
		"vec3 transformed_normals=normalize(mat3(u_view_matrix * u_model_matrix) * vNormal);" \
		"vec3 light_direction = normalize(vec3(u_light_position) - eye_coordinates.xyz);" \
		"float tn_dot_ld = max(dot(transformed_normals, light_direction),0.0);" \
		"vec3 ambient = u_La * u_Ka;" \
		"vec3 diffuse = u_Ld * u_Kd * tn_dot_ld;" \
		"vec3 reflection_vector = reflect(-light_direction, transformed_normals);" \
		"vec3 viewer_vector = normalize(-eye_coordinates.xyz);" \
		"vec3 specular = u_Ls * u_Ks * pow(max(dot(reflection_vector, viewer_vector), 0.0), u_material_shininess);" \
		"phong_ads_color=ambient + diffuse + specular;" \
		"}" \
		"else" \
		"{" \
		"phong_ads_color = vec3(1.0, 1.0, 1.0);" \
		"}" \
		"gl_Position=u_projection_matrix * u_view_matrix * u_model_matrix * vPosition;" \
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
		"\n" \
		"in vec3 phong_ads_color;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = vec4(phong_ads_color, 1.0);" \
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

	// Pre link binding of shader program object with fragment shader position attribute
	// This shader object is recognized by out YSG_ATTRIBUTE_NORMAL
	glBindAttribLocation(gShaderProgramObject, YSG_ATTRIBUTE_NORMAL, "vNormal");

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

	// Get uniform locations:
	// Uniforms for Model, View and Projection matrices:
	gModelMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_model_matrix");
	gViewMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_view_matrix");
	gProjectionMatrixUniform = glGetUniformLocation(gShaderProgramObject, "u_projection_matrix");

	// If L key is Pressed or not:
	gLKeyPressedUniform = glGetUniformLocation(gShaderProgramObject, "u_lighting_enabled");

	// Uniforms for Light:
	// Ambient color intensity of light:
	gLaUniform = glGetUniformLocation(gShaderProgramObject, "u_La");
	// Diffuse color intensity of light:
	gLdUniform = glGetUniformLocation(gShaderProgramObject, "u_Ld");
	// Specular color intensity of light:
	gLsUniform = glGetUniformLocation(gShaderProgramObject, "u_Ls");
	// Position of light:
	gLightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");;

	// Uniforms for material:
	// ambient reflective color intensity of material
	gKaUniform = glGetUniformLocation(gShaderProgramObject, "u_Ka");
	// diffuse reflective color intensity of material
	gKdUniform = glGetUniformLocation(gShaderProgramObject, "u_Kd");
	// specular reflective color intensity of material
	gKsUniform = glGetUniformLocation(gShaderProgramObject, "u_Ks");
	// shininess of material ( value is conventionally between 1 to 200 )
	gMaterialShininessUniform = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");;

	// Vertices, Colors, Shader attributes, vbo, vao initializations:
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

	// BLOCK FOR SPHERE:
	glGenVertexArrays(1, &gVaoSphere);
	glBindVertexArray(gVaoSphere);

	// A. BUFFER BLOCK FOR VERTICES:
	glGenBuffers(1, &gVboSpherePosition);					// Buffer to store vertex position
	glBindBuffer(GL_ARRAY_BUFFER, gVboSpherePosition);		// Find that named object in memory
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);	// Takes data from CPU to GPU

	glVertexAttribPointer(YSG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(YSG_ATTRIBUTE_POSITION);

	// Release the buffer for vertices:
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// B. BUFFER BLOCK FOR NORMALS:
	glGenBuffers(1, &gVboSphereNormal);					// Buffer to store vertex normals
	glBindBuffer(GL_ARRAY_BUFFER, gVboSphereNormal);		// Find that named object in memory
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);	// Takes data from CPU to GPU

	glVertexAttribPointer(YSG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(YSG_ATTRIBUTE_NORMAL);

	// C. BUFFER BLOCK FOR ELEMENTS:
	glGenBuffers(1, &gVboSphereElement);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVboSphereElement);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Release the buffer for colors:
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);				// Unbind

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glEnable(GL_CULL_FACE);			// Turn off culling for rotation

	// Set the background color to black:
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Set PerspectiveMatrix to identity matrix
	gPerspectiveProjectionMatrix = mat4::identity();

	// Warm up call. (Not required for Windows as resize is called before WM_PAINT
	resize(WIN_WIDTH, WIN_HEIGHT);
}

void display(void)
{
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Start to use the OpenGL program object
	glUseProgram(gShaderProgramObject);		// Compiled and linked program goes to the driver

	if (gbLight == true)
	{
		// Set the 'u_lighting_enabled' uniform:
		glUniform1i(gLKeyPressedUniform, 1);

		// Set the light's properties:
		glUniform3fv(gLaUniform, 1, lightAmbient);
		glUniform3fv(gLdUniform, 1, lightDiffuse);
		glUniform3fv(gLsUniform, 1, lightSpecular);
		glUniform4fv(gLightPositionUniform, 1, lightPosition);

		// Set the material's properties:
		glUniform3fv(gKaUniform, 1, materialAmbient);
		glUniform3fv(gKdUniform, 1, materialDiffuse);
		glUniform3fv(gKsUniform, 1, materialSpecular);
		glUniform1f(gMaterialShininessUniform, materialShininess);
	}
	else
	{
		// Set the 'u_lighting_enabled' uniform:
		glUniform1i(gLKeyPressedUniform, 0);
	}

	// OpenGL Drawing

	// CUBE:
	// Set the modelview, rotation, scale and modelviewprojection matrices to identity 
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();

	// Translate the modelViewMatrix along the z axis
	modelMatrix = translate(0.0f, 0.0f, -2.0f);

	glUniformMatrix4fv(gModelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(gViewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(gProjectionMatrixUniform, 1, GL_FALSE, gPerspectiveProjectionMatrix);

	// BIND vao
	glBindVertexArray(gVaoSphere);

	// Draw either by glDrawTraingles() or glDrawArrays() or glDrawElements()
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVboSphereElement);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	// Unbind vao
	glBindVertexArray(0);
	
	// Stop using the OpenGL program object
	glUseProgram(0);

	SwapBuffers(ghdc);
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

	// Destroy Sphere vao 
	if (gVaoSphere)
	{
		glDeleteVertexArrays(1, &gVaoSphere);
		gVaoSphere = 0;
	}

	// Destroy vbo for Sphere Position:
	if (gVboSpherePosition)
	{
		glDeleteBuffers(1, &gVboSpherePosition);
		gVboSpherePosition = 0;
	}

	// Destroy vbo for Sphere Normal:
	if (gVboSphereNormal)
	{
		glDeleteBuffers(1, &gVboSphereNormal);
		gVboSphereNormal = 0;
	}

	// Destroy vbo for Sphere Element:
	if (gVboSphereElement)
	{
		glDeleteBuffers(1, &gVboSphereElement);
		gVboSphereElement = 0;
	}

	// Detach the shaders first before deleting
	// Detach vertex shader from shader program object
	glDetachShader(gShaderProgramObject, gVertexShaderObject);
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
