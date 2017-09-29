//XWindows first program

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

//Namespaces:
using namespace std;

//Global variable declarations:
bool bFullscreen = false;		//Fullscreen variable
Display *gpDisplay = NULL;		//Global pointer
XVisualInfo *gpXVisualInfo = NULL;	
Colormap gColormap;
Window gWindow;

int giWindowWidth = 800;
int giWindowHeight = 600;

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
			switch(event.type)			//Switch on event type
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
	int defaultScreen;	//To get the default screen
	int defaultDepth;	//To get the default depth
	int styleMask;
	
	static int frameBufferAttributes[] = 
	{
		GLX_DOUBLEBUFFER, True,
		GLX_RGBA,
		GLX_RED_SIZE,8,
		GLX_GREEN_SIZE,8,
		GLX_BLUE_SIZE,8,
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
	
	defaultScreen = XDefaultScreen(gpDisplay);
	gpXVisualInfo = glXChooseVisual(gpDisplay, defaultScreen, frameBufferAttributes);
	//We have created a visual to create a new context
		
	winAttribs.border_pixel = 0;		//Take default value for color
	winAttribs.border_pixmap = 0;		//Take default pattern
	winAttribs.background_pixmap = 0;
	winAttribs.colormap = XCreateColormap(gpDisplay, RootWindow(gpDisplay, gpXVisualInfo->screen), gpXVisualInfo->visual, AllocNone);
	//Colormap is a h/w resource and is shared by the RootWindow. 
	//gpXVisualInfo->screen - We need visual screen and not default screen as rendering is done on this visual screen.
	//AllocNone - Do not allocate memory for color map. We need it only when there are child windows that share the color map of the parent.
	gColormap = winAttribs.colormap;
	
	winAttribs.background_pixel = BlackPixel(gpDisplay, defaultScreen);
	winAttribs.event_mask = ExposureMask | VisibilityChangeMask | ButtonPress | KeyPressMask | PointerMotionMask | StructureNotifyMask;
	//ExposureMask - Expose
	//VisibilityChangeMask - MapNotify 
	//ButtonPress - ButtonPress
	//KeyPressMask - KeyPress 
	//PointerMotionMask - MotionNotify
	//StructureNotifyMask - ConfigureNotify
	
	styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
	gWindow = XCreateWindow(gpDisplay,
				RootWindow(gpDisplay, gpXVisualInfo->screen),	//Parent window handle
				0,				//X
				0,				//Y
				giWindowWidth,			//W
				giWindowHeight,			//H
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
	gGLXContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, GL_TRUE);
	if(gGLXContext == NULL)
	{
		printf("ERROR: Failed to create Rendering Context.\n Exiting...\n");
		uninitialize();
		exit(1);
	}
	
	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	resize(giWindowWidth, giWindowHeight);		//Stray resize
} 

void display(void)
{
	void multicolored_triangle(void);

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	multicolored_triangle();

	glXSwapBuffers(gpDisplay, gWindow);

}

void resize(int width, int height)
{
	//code
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-50.0f, 50.0f, -50.0f, 50.0f, -50.0f, 50.0f);

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
}
		
void multicolored_triangle(void)
{
	// Triangle

	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 50.0f, 0.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-50.0f, -50.0f, 0.0f);

	glColor3f(0.0f, 0.0f, 50.0f);
	glVertex3f(50.0f, -50.0f, 0.0f);
	glEnd();

}



















