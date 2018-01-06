package com.yashg.window_hello2;			// Name of our package id which is unique to this program

// Default packages supplied by Androd SDK:
import android.app.Activity;				
import android.os.Bundle;					

// Packages added later:
import android.widget.TextView;				// For "TextView" class
import android.content.Context;				// For "Context" class
import android.graphics.Color;				// For "Color" class
import android.view.Gravity;				// For "Gravity" class

public class MyView extends TextView // Inheriting from parent class
{
    MyView(Context myContext)			// Activity gets type casted to context.
	{
		super(myContext);				// Set myContext to the parent
		setText("Hello World! (MyView)");		// Set 'hello world' text
		setTextSize(60);				// Set the size of the text
		setTextColor(Color.GREEN);		// Set the color for the text
		setGravity(Gravity.CENTER);		// Set the position of the text
	}
	
}
