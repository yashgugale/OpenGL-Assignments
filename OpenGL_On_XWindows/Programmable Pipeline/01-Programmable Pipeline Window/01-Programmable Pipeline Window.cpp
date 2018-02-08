//GLXContext with Frame Buffer Configurations
//Code for programmable pipeline
//Code for Vulcan programming

//Headers:
#include <iostream>	
#include <stdio.h>	//For print();
#include <stdlib.h>	//For exit();
#include <memory.h>	//For memset();

//XWindows server headers:
#include <X11/Xlib.h>	//Analogous to windows.h
#include <X11/Xutil.h>	//Used for XVisualInfo struct and associated APIs ie visuals
#include <X11/XKBlib.h>	//Keyboard library
#include <X11/keysym.h>	//For key symbols 

// GLEW Headers
#include <GL/glew.h>

//OpenGL headers:
#include <GL/gl.h>
#include <GL/glx.h>	//Bridging API

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//Namespaces:
using namespace std;

//Global variable declarations:
bool bFullscreen = false;		//Fullscreen variable
Display *gpDisplay = NULL;		//Global pointer
XVisualInfo *gpXVisualInfo = NULL;	
Colormap gColormap;
Window gWindow;

typedef GLXContext (*glXCreateContextAttribsARBProc) (Display*, GLXFBConfig, GLXContext, Bool, const int*);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;
GLXFBConfig gGLXFBConfig;
GLXContext gGLXContext;			//Rendering context. Parallel to HGLRC

FILE *gpFile = NULL;

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

	bool bDone = false;
	char ascii[26];		//For key press funtionality
	//Code:
	//Create log file:
	gpFile = fopen("Log_Programmable Pipeline.txt", "w");
	if(gpFile == NULL)
	{
		printf("Unable to create log file.\nExiting...\n");
		exit(1);
	}
	else
	{
		fprintf(gpFile, "Log file is successfully opened.\n");
	}
	
	
	//Create window:
	CreateWindow();		//Function to create a window
	
	//Initialize():
	initialize();
	
	//Game loop:
	XEvent event;		//To store the message/event. Parallel to MSG structure
	KeySym keysym;		
	int winWidth;
	int winHeight;

	while(bDone == false)
	{
		while(XPending(gpDisplay))
		{
			XNextEvent(gpDisplay, &event);		//Get next message. Fill 'event' with the address of next event. Parallel to GetMessage()
			switch(event.type)			//Switch on event type. Parallel to iMsg
			{
				case MapNotify:			//Parallel to WM_CREATE
					break;
				case KeyPress:			//Parallel to WM_KEYDOWN
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
						case 'f':
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
				case ButtonPress:
					switch(event.xbutton.button)
					{
						case 1:			//Left mouse button
							break;
						case 2:			//Middle mouse button
							break;	
						case 3:			//Right mouse button
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
				case 33:				//Close press
					bDone = true;
					break;
				default:
					break;
			}
		}
		
		display();
		//Spin call here
	}
	uninitialize();
	return(0);
}
				
void CreateWindow(void)
{
	//Function Prototypes:
	void uninitialize(void);
	
	//Variable declarations:
	XSetWindowAttributes winAttribs;
	GLXFBConfig *pGLXFBConfigs = NULL;	//Array pointer to store the returned visuals
	GLXFBConfig bestGLXFBConfig;		//Store the best visual
	XVisualInfo *pTempXVisualInfo = NULL;
	int iNumFBConfigs = 0;
	int styleMask;
	int i;
	
	//1. Our own specified Frame Buffer
	static int frameBufferAttributes[] = 
	{
		GLX_X_RENDERABLE, True,			//We want video rendering and not image rendering
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,	//Window is drawable
		GLX_RENDER_TYPE, GLX_RGBA_BIT,		//Frame is of RGBA type
		GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,	//Equivalent to giving TrueColor in XMatchVisualInfo
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DEPTH_SIZE, 24,
		GLX_STENCIL_SIZE, 8,
		GLX_DOUBLEBUFFER, True,
		//GLX_SAMPLE_BUFFERS, 1,
		//GLX_SAMPLES, 4,
		None			//As a stopper for array
	};
	
	//Code:
	gpDisplay = XOpenDisplay(NULL);		//Pointer to struct is the return value. It returns default display pointer.
	if(gpDisplay == NULL)
	{
		printf("ERROR: Unable to open X Display.\n Exiting...\n");
		uninitialize();
		exit(1);
	}
	
	//2. Get a new frame buffer config that meets our attrib requirements:
	pGLXFBConfigs = glXChooseFBConfig(gpDisplay, DefaultScreen(gpDisplay), frameBufferAttributes, &iNumFBConfigs);
	if(pGLXFBConfigs == NULL)
	{
		printf("Failed to get valid FrameBuffer Config.\nExiting now...\n");
		uninitialize();
		exit(1);
	}
	
	//3. As we are not asking the system to choose or match a visual, we get multiple visuals to choose from	
	printf("%d Matching FB Configs found.\n", iNumFBConfigs);
	
	//4. Pick that FB Config/Visual with the most samples per pixel
	int bestFramebufferConfig = -1, worstFramebufferConfig = -1, bestNumberOfSamples = -1, worstNumberOfSamples = 999;
	
	for(i = 0; i < iNumFBConfigs; i++)
	{
		pTempXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, pGLXFBConfigs[i]);
		if(pTempXVisualInfo)
		{
			int sampleBuffer, samples;
			glXGetFBConfigAttrib(gpDisplay, pGLXFBConfigs[i], GLX_SAMPLE_BUFFERS, &sampleBuffer);
			glXGetFBConfigAttrib(gpDisplay, pGLXFBConfigs[i], GLX_SAMPLES, &samples);
			printf("Matching Framebuffer Config = %d : Visual ID = 0x%lu : Sample_BUFFERS = %d : SAMPLES = %d\n", i, pTempXVisualInfo->visualid, sampleBuffer, samples);
			if(bestFramebufferConfig < 0 || sampleBuffer && samples > bestNumberOfSamples)
			{
				bestFramebufferConfig = i;
				bestNumberOfSamples = samples;
			}
			if(worstFramebufferConfig < 0 || !sampleBuffer || samples < worstNumberOfSamples)
			{
				worstFramebufferConfig = i;
				worstNumberOfSamples = samples;
			}
		}
		XFree(pTempXVisualInfo);
	}
	
	bestGLXFBConfig = pGLXFBConfigs[bestFramebufferConfig];
	
	//Set global GLXConfig
	gGLXFBConfig = bestGLXFBConfig;
	
	//Free the FBConfig list allocated by glXChooseFBConfig()
	XFree(pGLXFBConfigs);
	
	//5. 
	gpXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, bestGLXFBConfig);
	printf("Choose Visual ID = 0x%lu\n", gpXVisualInfo->visualid);
		
	winAttribs.border_pixel = 0;		//Take default value for color
	winAttribs.border_pixmap = 0;		//Take default pattern
	winAttribs.background_pixmap = 0;
	winAttribs.colormap = XCreateColormap(gpDisplay, RootWindow(gpDisplay, gpXVisualInfo->screen), gpXVisualInfo->visual, AllocNone);

	gColormap = winAttribs.colormap;
	
	winAttribs.event_mask = ExposureMask | VisibilityChangeMask | ButtonPress | KeyPressMask | PointerMotionMask | StructureNotifyMask;

	styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
	gWindow = XCreateWindow(gpDisplay,
				RootWindow(gpDisplay, gpXVisualInfo->screen),	//Parent window handle
				0,				//X
				0,				//Y
				WIN_WIDTH,			//W
				WIN_HEIGHT,			//H
				0,				//Thickness of border
				gpXVisualInfo->depth,		
				InputOutput,
				gpXVisualInfo->visual,
				styleMask,
				&winAttribs);
	if(!gWindow)
	{
		printf("ERROR: Failed to create Main Window.\n Exiting...\n");
		uninitialize();
		exit(1);
	}
	
	XStoreName(gpDisplay, gWindow, "First OpenGL X-Window");
	Atom windowManagerDelete = XInternAtom(gpDisplay, "WM_DELETE_WINDOW", True);	//33 turned on after this protocol
	XSetWMProtocols(gpDisplay, gWindow, &windowManagerDelete, 1);
	XMapWindow(gpDisplay, gWindow);		//ShowWindow(); UpdateWindow(); SetFocus(), etc
}

	
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

void initialize(void)
{
	//Function protoypes:
	void resize(int, int);
	void uninitialize(void);
	
	//Code:
	//6. Create a new GL context 4.5 for rendering:
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte*)"glXCreateContextAttribsARB");
	GLint attribs[] = 
	{
		GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
		GLX_CONTEXT_MINOR_VERSION_ARB, 5,
		GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
		0
	};
	
	gGLXContext = glXCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, attribs);
	if(!gGLXContext)	//On failure, fall back to old style 2.x context
	{
		//When a context version below 3.0 is requested, implementations will return the newest context version
		//compatible with openGL versions less than version 3.0
		GLint attribs[] = 
		{
			GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
			GLX_CONTEXT_MINOR_VERSION_ARB, 0,
			0
		};
		printf("Failed to create GLX 4.5 context. Hence, using old style GLX Context. \n");
		gGLXContext = glXCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, attribs);
	}
	else	//Successfully created 4.x context
	{
		printf("OpenGL Context 4.5 is created.\n");
	}
	
	//Verifying that context is a direct context
	if(!glXIsDirect(gpDisplay, gGLXContext))
	{
		printf("Indirect GLX rendering Context Obtained\n");
	}
	else
	{
		printf("Direct GLX Rendering Context Obtained\n");
	}

	
	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);
	
	// GLEW initialization code for GLSL. It must be here, ie after creating OpenGL context but before using any OpenGL function
	GLenum glew_error = glewInit();
	if(glew_error != GLEW_OK)
	{
		glXDestroyContext(gpDisplay, gGLXContext);
		gGLXContext = NULL;
		
		XCloseDisplay(gpDisplay);
		gpDisplay = NULL;
	}
	
	GLint number_of_extensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &number_of_extensions);	// Returns the number of OpenGL extensions available
	
	fprintf(gpFile, "\nPrinting the list of available extensions :\n");
	for(int i = 0; i < number_of_extensions; i++)
		fprintf(gpFile, "%d %s\n", i+1, (const char *)glGetStringi(GL_EXTENSIONS, i));
	fprintf(gpFile, "\n");
	
	glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
	
	glClearDepth(1.0f);		//clear depth buffer
	glEnable(GL_DEPTH_TEST);	//enable the depth
	glDepthFunc(GL_LEQUAL);		//less than or equal to 1.0f in far. Uses ray tracing algorithm

	glShadeModel(GL_SMOOTH);	//to remove aliasing
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	//to remove distortion
	glEnable(GL_CULL_FACE);
	
	resize(WIN_WIDTH, WIN_HEIGHT);
} 

void display(void)
{
	//Code:
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glXSwapBuffers(gpDisplay, gWindow);
}

void resize(int width, int height)
{
	//Code:
	if(height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}
	
void uninitialize(void)
{
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
	if(gpFile)
	{
		fprintf(gpFile, "Log file is successfully closed.\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
		




















