//XWindows Texture mapping

#include <iostream>	
#include <stdio.h>
#include <stdlib.h>	//For exit();
#include <memory.h>	//For memset();

//XWindows headers:

#include <X11/Xlib.h>
#include <X11/Xutil.h>	//Used for XVisualInfo struct and associated APIs
#include <X11/XKBlib.h>	//Keyboard library
#include <X11/keysym.h>	//For key symbols 

//OpenGL headers:
#include <GL/gl.h>
#include <GL/glx.h>	//Bridging API
#include <GL/glu.h>

//SOIL Header:
#include <SOIL/SOIL.h>

//Namespaces:
using namespace std;

//Global variable declarations:
bool bFullscreen = false;		//Fullscreen variable
Display *gpDisplay = NULL;		//Global pointer
XVisualInfo *gpXVisualInfo = NULL;	
Colormap gColormap;
Window gWindow;

int giWindowWidth = 800;		//Window width
int giWindowHeight = 600;		//Window height

GLfloat angle_pyramid = 0.0f; 	//pyramid rotation
GLfloat angle_cube = 0.0f; 		//cube rotation

GLuint Texture_Stone; 			//Stone texture object
GLuint Texture_Kundali; 		//Kundali texture object

GLXContext gGLXContext;			//Rendering context

//entry point fuction:
int main(void)		
{
	//Function prototypes:
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void initialize(void);
	void display(void);
	void resize(int, int);
	void uninitialize(void);
	void update(void);
	
	//Variable declarations:
	int winWidth = giWindowWidth;
	int winHeight = giWindowHeight;

	bool bDone = false;
	char ascii[26];		//For key press funtionality
	//Code:
	CreateWindow();		//Function to create a window
	
	//Initialize():
	initialize();
	
	XEvent event;		//To store the message/event
	KeySym keysym;		
	
	//Game loop
	while(bDone == false)
	{
		while(XPending(gpDisplay))
		{
			XNextEvent(gpDisplay, &event);		//Get next message. Fill 'event' with the address of next event.
			switch(event.type)					//Switch on event type
			{
				case MapNotify:
					break;
				case KeyPress:
					keysym = XkbKeycodeToKeysym(gpDisplay, event.xkey.keycode, 0, 0);
					switch(keysym)
					{
						case XK_Escape:			//Exit on Esc key press
							bDone = true;
						default:
							break;
					}
					XLookupString(&event.xkey, ascii, sizeof(ascii), NULL, NULL);
					switch(ascii[0])
					{
						case 'f':				//Toggle Fullscreen on keypress
						case 'F':
							if(bFullscreen == false)
							{
								ToggleFullscreen();
								bFullscreen = true;
							}
							else
							{
								ToggleFullscreen();
								bFullscreen = false;
							}
							break;
						default:
							break;
					}		
					break;
				//Declarations for future use
				case ButtonPress:
					switch(event.xbutton.button)
					{
						case 1:				//Left mouse button
							break;
						case 2:				//Middle mouse button
							break;	
						case 3:				//Right mouse button
							break;
						default:
							break;
					}
					break;
				case MotionNotify:			//Mouse move
					break;
				case ConfigureNotify:
					winWidth = event.xconfigure.width;
					winHeight = event.xconfigure.height;
					resize(winWidth, winHeight);
					break;
				case Expose:				//Painting of window
					break;
				case DestroyNotify:			
					break;
				case 33:					//Close press
					bDone = true;
					break;
				default:
					break;
			}
		}
		
		display();
		//Spin calls here
		update();
	}
	uninitialize();
	return(0);
}

//Create XWindows window				
void CreateWindow(void)
{
	//Function Prototypes:
	void uninitialize(void);
	
	//Variable declarations:
	XSetWindowAttributes winAttribs;
	int defaultScreen;	//To get the default screen
	int defaultDepth;	//To get the default depth
	int styleMask;
	
	//Set the depth parameters
	static int frameBufferAttributes[] = 
	{
		GLX_DOUBLEBUFFER, True,
		GLX_RGBA,
		GLX_RED_SIZE,8,
		GLX_GREEN_SIZE,8,
		GLX_BLUE_SIZE,8,
		GLX_DEPTH_SIZE, 24,
		None			//As a stopper for array
	};
	
	//Code:
	gpDisplay = XOpenDisplay(NULL);		//Pointer to struct is the return value. It returns default display pointer.
	if(gpDisplay == NULL)				//Error handling
	{
		printf("ERROR: Unable to open X Display.\n Exiting...\n");
		uninitialize();
		exit(1);
	}
	
	defaultScreen = XDefaultScreen(gpDisplay);
	gpXVisualInfo = glXChooseVisual(gpDisplay, defaultScreen, frameBufferAttributes);
	//We have created a visual to create a new context
		
	winAttribs.border_pixel = 0;		//Take default value for color
	winAttribs.border_pixmap = 0;		//Take default pattern
	winAttribs.background_pixmap = 0;
	winAttribs.colormap = XCreateColormap(gpDisplay, RootWindow(gpDisplay, gpXVisualInfo->screen), gpXVisualInfo->visual, AllocNone);

	gColormap = winAttribs.colormap;
	
	winAttribs.background_pixel = BlackPixel(gpDisplay, defaultScreen);
	winAttribs.event_mask = ExposureMask | VisibilityChangeMask | ButtonPress | KeyPressMask | PointerMotionMask | StructureNotifyMask;
	
	styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
	gWindow = XCreateWindow(gpDisplay,
				RootWindow(gpDisplay, gpXVisualInfo->screen),	//Parent window handle
				0,						//X
				0,						//Y
				giWindowWidth,			//W
				giWindowHeight,			//H
				0,						//Thickness of border
				gpXVisualInfo->depth,		
				InputOutput,
				gpXVisualInfo->visual,
				styleMask,
				&winAttribs);
	if(!gWindow)						//Error handling
	{
		printf("ERROR: Failed to create Main Window.\n Exiting...\n");
		uninitialize();
		exit(1);
	}
	
	XStoreName(gpDisplay, gWindow, "OpenGL 3D textures");
	Atom windowManagerDelete = XInternAtom(gpDisplay, "WM_DELETE_WINDOW", True);	//33 turned on after this protocol
	XSetWMProtocols(gpDisplay, gWindow, &windowManagerDelete, 1);
	XMapWindow(gpDisplay, gWindow);		//ShowWindow(); UpdateWindow(); SetFocus(), etc
}

//Toggle fullscreen	
void ToggleFullscreen(void)
{
	//Variable declarations:
	Atom wm_state;		//To save state of normal screen
	Atom fullscreen;	//TO save state of full screen
	XEvent xev = {0};
	
	//Code:
	wm_state = XInternAtom(gpDisplay, "_NET_WM_STATE", False);	//Network compliant. 
	memset(&xev, 0, sizeof(xev));		//Memset to default values
	
	xev.type = ClientMessage;		//Custom message
	xev.xclient.window = gWindow;		//Window for which client message is going(predefined message)
	xev.xclient.message_type = wm_state;	//Type of message that you are sending(created by you)
	xev.xclient.format = 32;		//byte size = 32
	xev.xclient.data.l[0] = bFullscreen ? 0 : 1;
	
	fullscreen = XInternAtom(gpDisplay, "_NET_WM_STATE_FULLSCREEN", False);
	xev.xclient.data.l[1] = fullscreen;
	
	XSendEvent(gpDisplay,
		RootWindow(gpDisplay, gpXVisualInfo->screen),	//Propagates to your window
		False,						//If the message be propagated to your children
		StructureNotifyMask,
		&xev);
		
}

//Initialization
void initialize(void)
{
	//Function protoypes:
	void resize(int, int);
	void uninitialize(void);
	int LoadGLTexture(GLuint *, char *);	//Load texture function prototype
	
	//Code:
	gGLXContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, GL_TRUE);
	if(gGLXContext == NULL)
	{
		printf("ERROR: Failed to create Rendering Context.\n Exiting...\n");
		uninitialize();
		exit(1);
	}
	
	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	glClearDepth(1.0f);									//clear depth buffer
	glEnable(GL_DEPTH_TEST);							//enable the depth
	glDepthFunc(GL_LEQUAL);								//less than or equal to 1.0f in far. Uses ray tracing algorithm

	glShadeModel(GL_SMOOTH);							//to remove aliasing
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	//to remove distortion

	//enable texture mapping
	glEnable(GL_TEXTURE_2D); 
	LoadGLTexture(&Texture_Kundali, (char *)"Kundali.bmp");	//cast the string constant to char *
	LoadGLTexture(&Texture_Stone, (char *)"Stone.bmp");		//To avoid warning of deprecated conversion from string constant to char*

	resize(giWindowWidth, giWindowHeight);					//Stray resize
} 

//Function to load the texture
int LoadGLTexture(GLuint *texture, char *path)
{
	//variable declarations
	unsigned char *image_data = NULL;
	int width, height;

	//code
	image_data = SOIL_load_image(path,
					&width,
					&height,
					0,
					SOIL_LOAD_RGB);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4); 	//pixel storage mode (word alignment/4 bytes)
	glGenTextures(1, texture); 				//1 image
	glBindTexture(GL_TEXTURE_2D, *texture); //bind texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//generate mipmapped texture (3 bytes, width, height & data from bmp)
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid *)image_data);	

	//Free the texture data
	SOIL_free_image_data(image_data);

	return 0;
}

//Drawing
void display(void)
{
	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*   Pyramid   */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-1.5f, 0.0f, -6.0f);
	glRotatef(angle_pyramid, 0.0f, 1.0f, 0.0f);

	glBindTexture(GL_TEXTURE_2D, Texture_Stone);

	glBegin(GL_TRIANGLES);

	//1. Front face
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f); 		// apex

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f); 	// left-bottom

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f); 		// right-bottom

    //2. Right face
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f); 		// apex

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f); 		// left-bottom

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f); 	// right-bottom

	//3. Back face
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f); 		// apex

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f); 	// left-bottom

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f); 	// right-bottom

    //4. left face
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f); 		// apex

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f); 	// left-bottom

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f); 	// right-bottom

	glEnd();

	/*    Kundali   */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.5f, 0.0f, -6.0f);
	glScalef(0.75f, 0.75f, 0.75f);
	glRotatef(angle_cube, 1.0f, 1.0f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, Texture_Kundali);

	glBegin(GL_QUADS);

	//1. Front face
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f); 		// right-top

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f); 		// left-top

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f); 	// left-bottom

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f); 		// right-bottom

    //2. Right face
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, -1.0f); 		// right-top

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f); 		// left-top

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f); 		// left-bottom

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f); 	// right-bottom

	//3. Back face
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f); 	// right-top

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f); 	// left-top

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f); 	// left-bottom

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, -1.0f); 		// right-bottom

    //4. Left face
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f); 		// right-top

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f); 	// left-top

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f); 	// left-bottom

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f); 	// right-bottom

    //5. Top face
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f); 		// right-TOP

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f); 	// left-top

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f); 		// left-bottom

	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f); 		// right-bottom

    //6. Bottom face
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f); 		// right-top

	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f); 	// left-top

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f); 	// left-bottom

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f); 	// right-bottom

	glEnd();

	// double bufferring
	glXSwapBuffers(gpDisplay, gWindow);


}

void resize(int width, int height)
{
	//Code:
	if(height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

//Update the drawing data
void update(void)
{
	// code
	//Rotate pyramid and cube 
	angle_pyramid = angle_pyramid + 0.03f;
	if (angle_pyramid >= 360.0f)
		angle_pyramid = 0.0f;

	angle_cube = angle_cube - 0.03f;
	if (angle_cube <= -360.0f)
		angle_cube = 0.0f;
}
	
//Free all resources
void uninitialize(void)
{

	// code
	//Delete the textures used
	if (Texture_Stone)
	{
		glDeleteTextures(1, &Texture_Stone);
		Texture_Stone = 0;
	}

	if (Texture_Kundali)
	{
		glDeleteTextures(1, &Texture_Kundali);
		Texture_Kundali = 0;
	}

	GLXContext currentGLXContext;
	currentGLXContext = glXGetCurrentContext();
	
	if(currentGLXContext != NULL && currentGLXContext == gGLXContext)
	{
		glXMakeCurrent(gpDisplay, 0, 0);
	}
	
	if(gGLXContext)
	{
		glXDestroyContext(gpDisplay, gGLXContext);
	}
	
	if(gWindow)
	{
		XDestroyWindow(gpDisplay, gWindow);
	}
	
	if(gColormap)
	{
		XFreeColormap(gpDisplay, gColormap);
	}
	
	if(gpXVisualInfo)
	{
		free(gpXVisualInfo);
		gpXVisualInfo = NULL;
	}
	
	if(gpDisplay)
	{
		XCloseDisplay(gpDisplay);
		gpDisplay = NULL;
	}
}
		




















