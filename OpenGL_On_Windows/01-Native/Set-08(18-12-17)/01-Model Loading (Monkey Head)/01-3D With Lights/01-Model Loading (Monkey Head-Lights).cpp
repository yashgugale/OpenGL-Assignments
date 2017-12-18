// Windows headers
#include <Windows.h>

// OpenGL headers
#include <gl/GL.h>
#include <gl/GLU.h>

// C headers
#include <stdio.h>
#include <stdlib.h>

// C++ headers
#include <vector>

// Symbolic constants

#define TRUE					1		// True is 1
#define FALSE					0		// False is 0

#define BUFFER_SIZE				256		// Maxinmum length of string in the mesh file
#define S_EQUAL					0		// Return value of strcmp when strings are equal

#define NUM_TEXTURE_COORDS		2		// Number of texture co-ordinates 
#define NUM_POINT_COORDS		3		// Number of point co-ordinates
#define NUM_FACE_TOKENS			3		// Minimum number of entries in face data
#define NUM_NORMAL_COORDS		3		// Number of normal coordinates

#define WIN_INIT_X				100		// X co-ordinate to top-left point
#define WIN_INIT_Y				100		// Y co-ordinate to top-left point
#define WIN_WIDTH				800		// Window width
#define WIN_HEIGHT				800		// Window height

#define VK_F					0x46	// Virtual key code of capital 'F' key
#define VK_f					0x60	// Virtual key code of small 'f' key

#define FOV_Y_ANGLE				45		// Field of view in Y direction

#define Z_NEAR					0.1		// Distance from viewer to near plane of viewing volume
#define Z_FAR					200.0	// Distance from viewer to far plane of viewing volume

#define VIEWPORT_BOTTOMLEFT_X	0		// X co-ordinate of bottom left point of viewport rectangle
#define VIEWPORT_BOTTOMLEFT_Y	0		// Y co-ordinate of bottom left point of viewport rectangle

#define MONKEYHEAD_X_TRANSLATE		0.0f	// X-translation of monkeyhead
#define MONKEYHEAD_Y_TRANSLATE		0.0f	// Y-translation of monkeyhead
#define MONKEYHEAD_Z_TRANSLATE		-5.0f	// Z-translation of monkeyhead

#define MONKEYHEAD_X_SCALE_FACTOR	1.5f	// X scale factor for monkeyhead
#define MONKEYHEAD_Y_SCALE_FACTOR	1.5f	// Y scale factor for monkeyhead
#define	MONKEYHEAD_Z_SCALE_FACTOR	1.5f	// Z scale factor for monkeyhead

#define START_ANGLE_POS				0.0f	// Marks the beginning angle position for rotation
#define END_ANGLE_POS				360.0f	// Marks the end angle position for rotation
#define	MONKEYHEAD_ANGLE_INCREMENT	1.0f	// Incrementing angle for monkeyhead

// Import libraries
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

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

bool gbLighting = false;

//Lighting arrays:
GLfloat light_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat light_diffuse[] = { 0.0f, 0.0f, 1.0f, 1.0f };
GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_position[] = { 2.0f, 1.0f, 1.0f, 0.0f };
GLfloat material_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat material_shininess[] = { 50.0f };

GLfloat g_rotate;

// Vector of vector of float to hold the vertex data
std::vector<std::vector<float>> g_vertices;

// Vector of vector of float to hold the texture data
std::vector<std::vector<float>> g_textures;

// Vector of vector of float to hold the normal data
std::vector<std::vector<float>>	g_normals;

// Vector of vector of int to hold index data in g_vertices
std::vector<std::vector<int>> g_face_tri, g_face_texture, g_face_normals;

// Handle a mesh file
FILE *g_fp_meshfile = NULL;

// Handle a log file
FILE *g_fp_logfile = NULL;

// Hold line in a file
char line[BUFFER_SIZE];

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
		TEXT("OpenGL 3D"),
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
				update(); //We call update function to rotate the triangle and square here.
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
	// Prototypes of functions called from initialize
	void resize(int, int);		// Handler to window resize event
	void uninitialize(void);	// Release and destroy resources
	void LoadMeshData(void);	// Load data from mesh file and populate global vectors

	// CODE
	g_fp_logfile = fopen("MONKEYHEADLOADER.LOG", "w");
	if (!g_fp_logfile)
		uninitialize();

	// Local definitions
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


	// Specifies clear value of the color buffer. 
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	// Black color

	// Add depth:
	// S1 : Enable depth test in the state machine
	// S2 : Specify the depth comparison function. GL_LEQUAL : passes if incoming z value is less than or equal to stored z value
	// S3 : Specifies the clear value of the depth buffer
	// S4 : Select shade model, FLAT or SMOOTH
	// S5 : Specify the implementation specific hint
	//		GL_PERSPECTIVE_CORRECTION_HINT : Indicates quality of color and texture coordinates obtained by interpolation
	//		GL_NICEST : The most correct, or the highest quality
	glEnable(GL_DEPTH_TEST);							// S1
	glDepthFunc(GL_LEQUAL);								// S2
	glClearDepth(1.0f);									// S3
	glShadeModel(GL_SMOOTH);							// S4
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// S5

	glEnable(GL_LIGHTING);

	//Lighting calls
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

	glEnable(GL_LIGHT0);

	// Read the mesh file and load the global vectors with appropriate data
	LoadMeshData();

	// Warm up call to resize
	resize(WIN_WIDTH, WIN_HEIGHT);
}

void resize(int width, int height)
{
	// If current height measures 0, then force it to 1 in order to avoid divide-by-zero error
	if (height == 0)
		height = 1;

	// Set the viewport transformation.
	// AP1, AP2 : (x,y) for lower left corner of the viewport rectangle
	// AP3, AP4 : Width-Height of viewport rectangle
	glViewport(VIEWPORT_BOTTOMLEFT_X, VIEWPORT_BOTTOMLEFT_Y, (GLsizei)width, (GLsizei)height);

	// Set the projection transformation:
	// S1 : Select projection matrix mode
	// S2 : Load identity matrix on projection matrix stack
	// S3 : Set perspective projection using gluPerspective
	//		AP1 : Field of view angle in y-direction in degrees
	//		AP2 : Aspect ratio
	//		AP3-AP4 : Distance from view to near clipping plane(AP3) & tofar clipping plane(AP4)
	glMatrixMode(GL_PROJECTION);													// S1
	glLoadIdentity();																// S2
	gluPerspective(FOV_Y_ANGLE, (GLfloat)width / (GLfloat)height, Z_NEAR, Z_FAR);	// S3

}

void LoadMeshData(void)
{
	void uninitialize(void);

	// Open the mesh file, name of the mesh file can be parameterized
	g_fp_meshfile = fopen("MONKEYHEAD.OBJ", "r");
	if (!g_fp_meshfile)
		uninitialize();

	// Separator strings
	// String holding space separator for strtok
	char *sep_space = " ";
	// String holding forward slash separator for strtok
	char *sep_fslash = "/";

	// Token pointers
	// Character pointer for holding the first word in a line
	char *first_token = NULL;
	// Character pointer for holding next word separated by the specified separator to strtok
	char *token = NULL;
	
	// Array of character pointers to hold strings of face entries
	// Face entries can be variable. In some files they are 3 and in some they are 4
	char *face_tokens[NUM_FACE_TOKENS];
	// Number of non-null tokens in the above vector
	int nr_tokens;

	// Character pointer for holding string associated with vertex index
	char *token_vertex_index = NULL;
	// Character pointer for holding string associated with texture index
	char *token_texture_index = NULL;
	// Character pointer for holding string associated with normal index
	char *token_normal_index = NULL;

	// While there is a line in the file
	while (fgets(line, BUFFER_SIZE, g_fp_meshfile) != NULL)
	{
		// Bind the line to a separator and get first token
		first_token = strtok(line, sep_space);						// First call to strtok. Hence, we have passed the line explicitly

		// If the first token indicates vertex data
		if (strcmp(first_token, "v") == S_EQUAL)
		{
			// Create a vector of NUM_POINT_COORDS number of floats to hold the coordinates
			std::vector<float> vec_point_coord(NUM_POINT_COORDS);

			// Do the following, NUM_POINT_COORDS number of times
			// S1 : Get next token
			// S2 : Feed it to atof to get the floating point number out of it
			// S3 : Add the floating point number generated to the vector
			// End of loop
			// S4 : At the end of the loop, vector is constructed, add it to the global vector of floats i.e g_vertices
			for (int i = 0; i != NUM_POINT_COORDS; i++)
				vec_point_coord[i] = atof(strtok(NULL, sep_space));			// S1, S2, S3. 2 or more calls to strtok for the same line. Hence, passed NULL
			g_vertices.push_back(vec_point_coord);							// S4
		}

		// If first token indicates texture data
		else if (strcmp(first_token, "vt") == S_EQUAL)
		{
			// Create a vector of NUM_TEXTURE_COORDS number of floats to hold the texture coordinates
			std::vector<float> vec_texture_coords(NUM_TEXTURE_COORDS);

			// Do the following, NUM_TEXTURE_COORDS number of times
			// S1 : Get next token
			// S2 : Feed it to atof to get the floating point number out of it
			// S3 : Add the floating point number generated to the vector
			// End of loop
			// S4 : At the end of the loop, vector is constructed, add it to the global vector of floats i.e g_textures
			for (int i = 0; i != NUM_TEXTURE_COORDS; i++)
				vec_texture_coords[i] = atof(strtok(NULL, sep_space));		// S1, S2, S3. 2 or more calls to strtok for the same line. Hence, passed NULL
			g_textures.push_back(vec_texture_coords);						// S4
		}

		// If first token indicates normal data
		else if (strcmp(first_token, "vn") == S_EQUAL)
		{
			// Create a vector of NUM_NORMAL_COORDS number of floats to hold the normal coordinates
			std::vector<float> vec_normal_coords(NUM_NORMAL_COORDS);

			// Do the following, NUM_NORMAL_COORDS number of times
			// S1 : Get next token
			// S2 : Feed it to atof to get the floating point number out of it
			// S3 : Add the floating point number generated to the vector
			// End of loop
			// S4 : At the end of the loop, vector is constructed, add it to the global vector of floats i.e g_normals
			for (int i = 0; i != NUM_NORMAL_COORDS; i++)
				vec_normal_coords[i] = atof(strtok(NULL, sep_space));		// S1, S2, S3. 2 or more calls to strtok for the same line. Hence, passed NULL
			g_normals.push_back(vec_normal_coords);						// S4
		}

		// If first token indicates face data
		else if (strcmp(first_token, "f") == S_EQUAL)
		{
			// Define 3 vectors of integers with length 3 to hold indices of triangle's positional coordinates, texture 
			// coordinates and normal coordinates in g_vertices, g_textures and g_normals respectively.
			std::vector<int> triangle_vertex_indices(3), texture_vertex_indices(3), normal_vertex_indices(3);

			// Initialize all char pointers in face_tokens to NULL
			memset((void *)face_tokens, 0, NUM_FACE_TOKENS);

			// Extract three fields of information in face_tokens and increment nr_tokens accordingly
			nr_tokens = 0;
			while (token = strtok(NULL, sep_space))
			{
				if (strlen(token) < 3)
					break;
				face_tokens[nr_tokens] = token;
				nr_tokens++;
			}

			// Every face data entry is going to have minimum 3 fields, hence construct a triangle out of it
			// S1 : triangle coordinate index data
			// S2 : texture coordinate index data
			// S3 : normal coordinate index data
			// S4 : Put the data in triangle_vertex_indices, texture_vertex_indices and normal_vertex_indices
			// Vectors will be constructed at the end of the loop
			for (int i = 0; i != NUM_FACE_TOKENS; i++)
			{
				token_vertex_index = strtok(face_tokens[i], sep_fslash);			// S1
				token_texture_index = strtok(NULL, sep_fslash);						// S2
				token_normal_index = strtok(NULL, sep_fslash);						// S3
				triangle_vertex_indices[i] = atoi(token_vertex_index);				// S4 - 1
				texture_vertex_indices[i] = atoi(token_texture_index);				// S4 - 2
				normal_vertex_indices[i] = atoi(token_normal_index);				// S4 - 3
			}

			// Add constructed vectors to global face vectors
			g_face_tri.push_back(triangle_vertex_indices);
			g_face_texture.push_back(texture_vertex_indices);
			g_face_normals.push_back(normal_vertex_indices);
		}

		// Initialize line buffer to NULL
		memset((void *)line, (int)'\0', BUFFER_SIZE);
	}

	// Close meshfile and make file pointer NULL
	fclose(g_fp_meshfile);
	g_fp_meshfile = NULL;

	// Log vertex, texture, normal and face data in a log file opened earlier
	fprintf(g_fp_logfile, "g_vertices: %u g_texture: %u g_normals: %u g_face_tri: %u\n",
		g_vertices.size(), g_textures.size(), g_normals.size(), g_face_tri.size());

}

void update(void)
{
	// Increment monkeyhead rotation angle by MONKEYHEAD_ANGLE_INCREMENT
	g_rotate = g_rotate + MONKEYHEAD_ANGLE_INCREMENT;

	// If rotation angle equals or exceeds END_ANGLE_POS, then reset the START_ANGLE_POS
	if (g_rotate >= END_ANGLE_POS)
		g_rotate = START_ANGLE_POS;
}

void display(void)
{
	void uninitialize(void);		// Release and destroy resources

	// Clear color and depth buffers by their clear values
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Select MODELVIEW matrix stack and load identity matrix in it
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Model transformations
	// S1 : Translate the MONKEYHEAD in X, Y, Z directions respectively
	// S2 : Rotate the MONKEYHEAD around Y axis by angle maintained in gl_rotate
	// S3 : Perform scaling transformation along X, Y, Z dimensions respectively
	glTranslatef(MONKEYHEAD_X_TRANSLATE, MONKEYHEAD_Y_TRANSLATE, MONKEYHEAD_Z_TRANSLATE);				// S1
	glRotatef(g_rotate, 0.0f, 1.0f, 0.0f);																// S2				
	glScalef(MONKEYHEAD_X_SCALE_FACTOR, MONKEYHEAD_Y_SCALE_FACTOR, MONKEYHEAD_Z_SCALE_FACTOR);			// S3

	// Keep counter clockwise winding of vertices geometry while drawing
	glFrontFace(GL_CCW);

	// Set Polygon mode mentioning front and back face and GL_LINE
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);		// Or GL_FILL for 3D

	// S1 : for every face index maintained in triangular form in g_face_tri, do the following
	//		S2 : Set geometry primitive to GL_TRIANGLES
	//		S3 : Extract triangle from outer loop index
	//		for every point of a triangle
	//			S4 : Calculate the index in g_vertices in variable vi
	//			S5 : Calculate x, y, z coordinates of point
	//			S6 : Send to glVertex3f
	// Remark : In S4, we have to subtract g_face_tri[i][j] by 1, because in mesh file, indexing starts 
	// from 1 and in case of arrays/vectors, indexing starts from 0

	for (int i = 0; i != g_face_tri.size(); i++)			// S1
	{
		glBegin(GL_TRIANGLES);								// S2
		glColor3f(1.0f, 0.5f, 0.0f);
		for (int j = 0; j != g_face_tri[i].size(); j++)		// S3
		{
			int vi = g_face_tri[i][j] - 1;					// S4
			glNormal3f(g_normals[vi][0], g_normals[vi][1], g_normals[vi][2]);
			glVertex3f(g_vertices[vi][0], g_vertices[vi][1], g_vertices[vi][2]);		// S5, S6
		}
		glEnd();
	}

	// Bring background framebuffer to foreground and make current foreground buffer available for rendering
	SwapBuffers(ghdc);

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



	// Close log file and make file pointer NULL
	fclose(g_fp_logfile);
	g_fp_logfile = NULL;

	DestroyWindow(ghwnd);
	ghwnd = NULL;
	
}
