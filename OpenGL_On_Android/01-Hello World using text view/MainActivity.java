package com.yashg.window_hello1;			// Name of our package id which is unique to this program

// Default packages supplied by Androd SDK:
import android.app.Activity;				
import android.os.Bundle;					

// Packages added later:
import android.view.Window;					// For "Window" class
import android.view.WindowManager;			// For "WindowManager" class
import android.content.pm.ActivityInfo;		// For "ActivityInfo" class
import android.widget.TextView;				// For "TextView" class
import android.graphics.Color;				// For "Color" class
import android.view.Gravity;				// For "Gravity" class
// 'pm' is the package manager.

public class MainActivity extends Activity // Inheriting from parent class
{
    @Override
    protected void onCreate(Bundle savedInstanceState) 
	{
		// It is a good practice to call super as the first statement
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
		
		// 3. Centrally located Hello World
		TextView myTextView = new TextView(this);	// We are passing this for Activity 
		myTextView.setText("Hello World !");		// Text to be displayed
		myTextView.setTextSize(60);					// Size of the text
		myTextView.setTextColor(Color.GREEN);		// Color of the text
		myTextView.setGravity(Gravity.CENTER);		// Centrally located text
		
		setContentView(myTextView);					// Set the view (in the same file).
		// SetContentView() is the last line.
	}
}
