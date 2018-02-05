package com.yashg.triangle_multicolored;		// Multicolored Perspective Triangle

import android.app.Activity;					// For "Activity" class
import android.os.Bundle;						// For "Bundle" class

import android.view.Window;						// For "Window" class
import android.view.View;						// For "View" class
import android.view.WindowManager;				// For "WindowManager" class
import android.content.pm.ActivityInfo;			// For "ActivityInfo" class

public class MainActivity extends Activity
{
	private GLESView glesView;
	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		
		// Remove the Action Bar:
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		
		// Hide the navigation bar (Optional):
		//View decorView = getWindow().getDecorView();
		//int uiOptions = View.SYSTEM_UI_FLAG_HIDE_NAVIGATION;
		//decorView.setSystemUiVisibility(uiOptions);

		// Make full screen:
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		
		// Set orientation to Landscape:
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		
		glesView = new GLESView(this);
		
		// Set view as the content view of the activity:
		setContentView(glesView);
	}
	
	@Override
	protected void onPause()
	{
		super.onPause();
	}
	
	@Override
	protected void onResume()
	{
		super.onResume();
	}
}