package com.yashg.event_handling;			// Name of our package id which is unique to this program

// Default packages supplied by Androd SDK:
import android.app.Activity;				
import android.os.Bundle;					

// Packages added later:
import android.widget.TextView;				// For "TextView" class
import android.content.Context;				// For "Context" class
import android.graphics.Color;				// For "Color" class
import android.view.Gravity;				// For "Gravity" class
import android.view.MotionEvent;			// For "MotionEvent" class
import android.view.GestureDetector;		// For GestureDetector
import android.view.GestureDetector.OnGestureListener;		// For OnGestureListener
import android.view.GestureDetector.OnDoubleTapListener;	// For OnDoubleTapListener

// Inheriting from parent class (partially overriding parent class and implementing for 
// OnGestureListener and OnDoubleTapListener class.
public class MyView extends TextView implements OnGestureListener, OnDoubleTapListener
{
	private GestureDetector gestureDetector;
    MyView(Context myContext)			// Activity gets type casted to context.
	{
		super(myContext);					// Set myContext to the parent (partially overridable)
		setText("Hello World! (MyView)");	// Set 'hello world' text
		setTextSize(60);				// Set the size of the text
		setTextColor(Color.GREEN);		// Set the color for the text
		setGravity(Gravity.CENTER);		// Set the position of the text
		
		// myContext = all global information (environment variables)
		// this = which class is using this object
		// null = handler to our own methods class (completion handler)
		// false = unused parameter
		gestureDetector = new GestureDetector(myContext, this, null, false);
		
		gestureDetector.setOnDoubleTapListener(this);	// handler is set here
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
		setText("Double tap");		// Single finger double tap
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
		setText("Single Tap");		// Decides single tap based on the time
		return(true);				// between two taps
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
		return(true);		
	}	
	
	// Abstract method from OnGestureListener, so it must be implemented
	@Override
	public void onLongPress(MotionEvent e)
	{
		setText("Long Press");
	}
	
	// Abstract method from OnGestureListener, so it must be implemented
	// distanceX = start pos, distanceY = end pos (distance is imp in scroll)
	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
	{
		setText("Scroll");
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
}
