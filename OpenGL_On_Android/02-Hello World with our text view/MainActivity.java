package com.yashg.window_hello2;		// Different package ID for a new program

import android.app.Activity;			// Use "Activity" class
import android.os.Bundle;				// Use "Bundle"

// Packages added later:
import android.view.Window;					// For "Window" class
import android.view.WindowManager;			// For "WindowManager" class
import android.content.pm.ActivityInfo;		// For "ActivityInfo" class
import android.graphics.Color;				// For "Color" class

public class MainActivity extends Activity 
{
	private MyView myView;	// Can also be local. But in OpenGL we need it in scope of class
	
    @Override
    protected void onCreate(Bundle savedInstanceState) 
	{	
        super.onCreate(savedInstanceState);

		// 1. Make window fullscreen
		/* 
			this is done to get rid of action bar. 'this' is not compulsary here
			Anything following '.' without '()' can be either of:
			(Caps) 1. constant OR 2. enum OR (Camel notation) 3. nested/inner class
		*/
		this.requestWindowFeature(Window.FEATURE_NO_TITLE);	
		
		// this is done to make fullscreen. (We use object chaining here)
		// Layout is in the hands of WindowManager.
		this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
			WindowManager.LayoutParams.FLAG_FULLSCREEN);
			
		// 2. Change orientation as Landscape. 
		MainActivity.this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		
		// Set the background color to black (Similar to hbrush)
		// DecorView is the main view of the window (root view in Mac)
		// We can also use : 'BLACK' as the parameter to SetBackgroundColor().
		this.getWindow().getDecorView().setBackgroundColor(Color.rgb(0,0,0));	

		// Above are the 4 features that we are using: 
		// 1. title bar, fullscreen, landscape, black background

		myView = new MyView(this);		// Create an object
		setContentView(myView);			// Set the content as the new view
		
	}
}
