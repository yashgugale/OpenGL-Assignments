package com.yashg.gles_view;			// Name of our package id which is unique to this program

// Default packages supplied by Androd SDK:
import android.app.Activity;				
import android.os.Bundle;					

// Packages added later:
import android.widget.TextView;				// For "TextView" class
import android.content.Context;				// For "Context" class

import android.opengl.GLSurfaceView;		// For OpenGL Surface View and related
import android.opengl.GLES32;				// For OpenGL ES 3.2 (can be 3.1/3.0 etc)

import javax.microedition.khronos.opengles.GL10;	// For OpenGL ES 1.0 needed as param type GL10
import javax.microedition.khronos.egl.EGLConfig;	// For EGLConfig needed as param type EGLConfig

import android.view.MotionEvent;			// For "MotionEvent" class
import android.view.GestureDetector;		// For GestureDetector
import android.view.GestureDetector.OnGestureListener;		// For OnGestureListener
import android.view.GestureDetector.OnDoubleTapListener;	// For OnDoubleTapListener

// A view for OpenGL ES 3 graphics which also receives touch events
public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
	private final Context myContext;
	private GestureDetector gestureDetector;
	
    public GLESView(Context drawingContext)	// Activity gets type casted to context. Constructor
	{
		super(drawingContext);				// Set super
		myContext = drawingContext;			
		
		// Accordingly set EGLContext to current supported version of OpenGL-ES
		// Tells NDK to create context of version 3.0 (no need to give subversion)
		setEGLContextClientVersion(3);		// EGL indicates the use of NDK (Version negotiation step)
		
		//Set Renderer for drawing on the GLSurfaceView
		setRenderer(this);			// Tells your view to go to context.
		
		//Render the view only when there is change in the drawing data
		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);	// Similar to repaint (On screen rendering)
		
		// myContext = all global information (environment variables)
		// this = which class is using this object
		// null = handler to our own methods class (completion handler)
		// false = unused parameter
		gestureDetector = new GestureDetector(myContext, this, null, false);
		
		gestureDetector.setOnDoubleTapListener(this);	// handler is set here
	}
	
	// Overriden method of GLSurfaceView.Renderer (INIT CODE)
	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig eglConfig)
	{
		// OpenGL ES version check
		String version = gl.glGetString(GL10.GL_VERSION);	// Calling on the object of GL10
		System.out.println("YSG : " + version);				// Prints the version name
		initialize(gl);										// Our function
	}
	
	// Overriden method of GLSurfaceView.Renderer (RESIZE CODE)
	@Override
	public void onSurfaceChanged(GL10 unused, int width, int height)
	{
		resize(width, height);
	}
	
	// Overriden method for GLSurfaceView.Renderer (RENDERING / DRAWING CODE)
	@Override
	public void onDrawFrame(GL10 unused)
	{
		display();
	}
	
	// Handling 'onTouchEvent' is most important because it triggers all gesture & tap events
	@Override
	public boolean onTouchEvent(MotionEvent event)
	{
		// Put switch on actions (e.g. Action.up/Action.down)
		int eventaction = event.getAction();		// Not used here
		if(!gestureDetector.onTouchEvent(event))	// If event from Gesture is not used,
			super.onTouchEvent(event);				// then use from my event
		return(true);	
	}
	
	// Abstract method from OnDoubleTapListener, so it must be implemented
	@Override
	public boolean onDoubleTap(MotionEvent e)
	{
		System.out.println("YSG : " + "Double tap");		// Single finger double tap
		return(true);
	}
	
	// Abstract method from OnDoubleTapListener, so it must be implemented
	@Override
	public boolean onDoubleTapEvent(MotionEvent e)
	{
		// No code, as already written in 'OnDoubleTap'
		// true indicates that we are interested in the message
		// false indicates that we are not interested in the message
		return(true);	
	}
	
	// Abstract method from OnDoubleTapListener, so it must be implemented
	@Override
	public boolean onSingleTapConfirmed(MotionEvent e)
	{
		System.out.println("YSG : " + "Single Tap");	// Decides single tap based on the time
		return(true);									// between two taps
	}
	
	
	// Abstract method from OnGestureListener, so it must be implemented
	@Override
	public boolean onDown(MotionEvent e)
	{
		// No code, as already written in 'onSingleTapConfirmed'
		return(true);		// Finger and also pen tap
	}
	
	// Abstract method from OnGestureListener, so it must be implemented
	// velocityX = start pos, velocityY = end pos (Speed is imp in swipe)
	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY)
	{
		System.out.println("YSG : " + "Swipe");			// Swipe/Fling event on the screen
		return(true);		
	}	
	
	// Abstract method from OnGestureListener, so it must be implemented
	@Override
	public void onLongPress(MotionEvent e)
	{
		System.out.println("YSG : " + "Long Press");
	}
	
	// Abstract method from OnGestureListener, so it must be implemented
	// distanceX = start pos, distanceY = end pos (distance is imp in scroll)
	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
	{
		System.out.println("YSG : " + "Scroll");		// We have decided to terminate the application
		System.exit(0);									// on a scroll event
		return(true);
	}
	
	// Abstract method from OnGestureListener, so it must be implemented
	@Override
	public void onShowPress(MotionEvent e)
	{
		//return(true);
	}
	
	// Abstract method from OnGestureListener, so it must be implemented
	@Override
	public boolean onSingleTapUp(MotionEvent e)
	{
		return(true);		// For key up
	}
	
	// For OpenGL functions, use GLES32/GLES31/GLES30 etc, based on the version supported on the mobile device
	private void initialize(GL10 gl)		// Init function (User defined)
	{
		// Set the background color
		GLES32.glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	}
	
	private void resize(int width, int height)
	{
		// Adjust the viewport based on the geometry changes, such as screen rotation, etc
		GLES32.glViewport(0, 0, width, height);
	}
	
	public void display()
	{
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
	}
}
