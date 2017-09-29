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

//entry point fuction:
int main(void)		
{
	//Function prototypes:
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void uninitialize(void);

	//Variable declarations:
	int winWidth = giWindowWidth;
	int winHeight = giWindowHeight;

	//Code:
	CreateWindow();		//function to create a window
	
	//Message Loop:
	XEvent event;		//To store the message/event
	KeySym keysym;		
	
	while(1)
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
						uninitialize();
						exit(0);
					case XK_F:			//F/f keypress for fullscreen
					case XK_f:
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
				break;
			case Expose:				//Painting of window
				break;
			case DestroyNotify:			
				break;
			case 33:				//Close press
				uninitialize();
				exit(0);
			default:
				break;
		}
	}
	
	uninitialize();		//We never enter here in OpenGL. Configuration for Direct 3D
	exit(0);
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
	
	//Code:
	gpDisplay = XOpenDisplay(NULL);		//Pointer to struct is the return value. It returns default display pointer.
	if(gpDisplay == NULL)
	{
		printf("ERROR: Unable to open X Display.\n Exiting...\n");
		uninitialize();
		exit(1);
	}
	
	defaultScreen = XDefaultScreen(gpDisplay);
	defaultDepth = DefaultDepth(gpDisplay, defaultScreen);
	gpXVisualInfo = (XVisualInfo *)malloc(sizeof(XVisualInfo));
	
	if(gpXVisualInfo == NULL)
	{
		printf("ERROR: Unable to allocate memory for Visual Info.\n Exiting...\n");
		uninitialize();
		exit(1);
	}	

	if(XMatchVisualInfo(gpDisplay, defaultScreen, defaultDepth, TrueColor, gpXVisualInfo) == 0)
	{
		printf("ERROR: Unable to get a Visual.\n Exiting...\n");
		uninitialize();
		exit(1);
	}

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
	
	XStoreName(gpDisplay, gWindow, "First X-Window");
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

void uninitialize(void)
{
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
		




















