package com.yashg.kundali_stone_texture;						// For kundali and stone texture

import android.content.Context;									// For "Context" class

import android.opengl.GLSurfaceView;							// For "GLSurfaceView" class
import android.opengl.GLES32;									// For OpenGL ES 32

import javax.microedition.khronos.opengles.GL10;				// For OpenGLES 1.0 needed as param
import javax.microedition.khronos.egl.EGLConfig;				// For EGLConfig needed as param

import android.view.MotionEvent;								// For "MotionEvent" class
import android.view.GestureDetector;							// For "GestureDetector" class
import android.view.GestureDetector.OnGestureListener;			// For "OnGestureListener" class
import android.view.GestureDetector.OnDoubleTapListener;		// For "OnDoubleTapListener" class

// For vbo:
import java.nio.ByteOrder;
import java.nio.ByteBuffer;
import java.nio.FloatBuffer;

import android.opengl.Matrix;									// For Matrix math;

import android.graphics.BitmapFactory;							// Texture factory
import android.graphics.Bitmap;									// For PNG image
import android.opengl.GLUtils;									// For texImage2D()

// A view for OpenGL ES 3 graphics which also receives touch events:
public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener
{
	private final Context myContext;
	private GestureDetector myGestureDetector;

	// OpenGL objects:
	private int VertexShaderObject;
	private int FragmentShaderObject;
	private int ShaderProgramObject;
	
	// vao and vbo:
	private int[] vaoPyramid = new int[1];
	private int[] vboPyramidPosition = new int[1];
	private int[] vboPyramidTexture = new int[1];
	
	private int[] vaoCube = new int[1];
	private int[] vboCubePosition = new int[1];
	private int[] vboCubeTexture = new int[1];
	
	private int mvpUniform;
	private int texture0_sampler_uniform;			// Texture sampler
	
	// Texture objects:
	private int[] texture_kundali = new int[1];
	private int[] texture_stone = new int[1];
	
	private float PerspectiveProjectionMatrix[] = new float[16];		// 4x4 matrix
	
	// Rotation Variables:
	private float anglePyramid = 0.0f;
	private float angleCube = 0.0f;
	
	// Constructor:
	public GLESView(Context drawingContext)
	{
		super(drawingContext);
		myContext = drawingContext;
		
		// Set the EGLContext to current supported version of OpenGL-ES:
		setEGLContextClientVersion(3);
		
		// Set the renderer:
		setRenderer(this);
		
		// Set the render mode to render only when there is change in the drawing data:
		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
		
		myGestureDetector = new GestureDetector(myContext, this, null, false);
		myGestureDetector.setOnDoubleTapListener(this);
	}
	
	// Initialize function:
	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config)
	{
		// Print OpenGL ES version:
		String glesVersion = gl.glGetString(GL10.GL_VERSION);
		System.out.println("YSG : OpenGL ES Version : " + glesVersion);
		
		// Print OpenGL Shading Language version:
		String glslVersion = gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION);
		System.out.println("YSG : OpenGL Shading Language Version : " + glslVersion);
		
		// Print supported OpenGL extensions:
		//String glesExtensions = gl.glGetString(GL10.GL_EXTENSIONS);
		//System.out.println("YSG : OpenGL Extensions supported on this device : " + glesExtensions);
		
		initialize(gl);
	}
	
	// Resize funtion:
	// The function signatures are kept same and thus have 'GL10' as a parameter. In OpenGL 3.0, GL10 is not used.
	// Yet, to keep the code simple, the signature is the same.
	@Override
	public void onSurfaceChanged(GL10 unused, int width, int height)
	{
		resize(width, height);
	}
	
	// Display function:
	// The function signatures are kept same and thus have 'GL10' as a parameter. In OpenGL 3.0, GL10 is not used.
	// Yet, to keep the code simple, the signature is the same.
	@Override
	public void onDrawFrame(GL10 unused)
	{
		display();
	}
	
	// 0. Handle 'onTouchEvent' because it triggers all gesture and tap events:
	@Override
	public boolean onTouchEvent(MotionEvent e)
	{
		int eventAction = e.getAction();
		if(!myGestureDetector.onTouchEvent(e))
			super.onTouchEvent(e);
		return(true);
	}
	
	// 1. Double Tap:
	@Override
	public boolean onDoubleTap(MotionEvent e)
	{
		return(true);
	}
	
	// 2. Double Tap event:
	@Override
	public boolean onDoubleTapEvent(MotionEvent e)
	{
		return(true);
	}
	
	// 3. Single Tap:
	@Override
	public boolean onSingleTapConfirmed(MotionEvent e)
	{
		return(true);
	}
	
	// 4. On Down:
	@Override
	public boolean onDown(MotionEvent e)
	{
		return(true);
	}
	
	// 5. On Single Tap UP:
	@Override
	public boolean onSingleTapUp(MotionEvent e)
	{
		return(true);
	}
	
	// 6. Fling:
	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY)
	{
		return(true);
	}
	
	// 7. Scroll: (Exit the program)
	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
	{
		uninitialize();
		System.exit(0);
		return(true);
	}
	
	// 8. Long Press:
	@Override
	public void onLongPress(MotionEvent e)
	{
	}
	
	// 9. Show Press:
	@Override
	public void onShowPress(MotionEvent e)
	{
	}
	
	private void initialize(GL10 gl)
	{
		/*<-- VERTEX SHADER -->*/
		
		// Create vertex shader
		VertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		
		// Vertex shader source code:
		final String vertexShaderSourceCode = String.format
		(
			"#version 320 es"+
			"\n"+
			"in vec4 vPosition;"+
			"in vec2 vTexture0_Coord;"+
			"out vec2 out_texture0_coord;"+
			"uniform mat4 u_mvp_matrix;"+
			"void main(void)"+
			"{"+
			"gl_Position = u_mvp_matrix * vPosition;"+
			"out_texture0_coord = vTexture0_Coord;"+
			"}"
		);
		
		// Provide source code to the vertex shader:
		// Specify the shader object and the corresponding vertex shader text (ie source code)
		GLES32.glShaderSource(VertexShaderObject, vertexShaderSourceCode);
		
		// Compile the shader and check for any errors:
		GLES32.glCompileShader(VertexShaderObject);
		
		// Error checking:
		int[] iShaderCompilationStatus = new int[1];	// 1 member integer array
		int[] iInfoLogLength = new int[1];				// 1 member integer array
		String szInfoLog = null;						// String to store the log
		
		// As there are no addresses in Java, we use an array of size 1
		GLES32.glGetShaderiv(VertexShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompilationStatus, 0);
		if(iShaderCompilationStatus[0] == GLES32.GL_FALSE)	// Compilation failed
		{
			GLES32.glGetShaderiv(VertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			if(iInfoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(VertexShaderObject);
				System.out.println("YSG : Vertex Shader Compilation Log = "+szInfoLog);
				uninitialize();
				System.exit(0);
			}
		}
		
		/*<-- FRAGMENT SHADER -->*/
		
		// Create Fragment Shader:
		FragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
		
		// Fragment shader source code:
		final String FragmentShaderSourceCode = String.format
		(
			"#version 320 es"+
			"\n"+
			"precision highp float;"+
			"in vec2 out_texture0_coord;"+
			"uniform highp sampler2D u_texture0_sampler;"+
			"out vec4 FragColor;"+
			"void main(void)"+
			"{"+
			"FragColor = texture(u_texture0_sampler, out_texture0_coord);"+
			"}"
		);
		
		// Provide source code to the Fragment shader:
		// Specify the shader object and the corresponding fragment shader text (ie source code)
		GLES32.glShaderSource(FragmentShaderObject, FragmentShaderSourceCode);
		
		// Compile the shader and check for any errors:
		GLES32.glCompileShader(FragmentShaderObject);
		
		// Error checking:
		iShaderCompilationStatus[0] = 0;			// Re initialize
		iInfoLogLength[0] = 0;						// Re initialize
		szInfoLog = null;							// Re initialize
		
		GLES32.glGetShaderiv(FragmentShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompilationStatus, 0);
		if(iShaderCompilationStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(FragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			if(iInfoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(FragmentShaderObject);
				System.out.println("YSG : Fragment shader compilation log = "+szInfoLog);
				uninitialize();
				System.exit(0);
			}
		}
		
		// Create the Shader Program:
		ShaderProgramObject = GLES32.glCreateProgram();
		
		// Attach Vertex shader to the shader program:
		GLES32.glAttachShader(ShaderProgramObject, VertexShaderObject);
		
		// Attach the Fragment shader to the shader program:
		GLES32.glAttachShader(ShaderProgramObject, FragmentShaderObject);
		
		// Pre-link binding of shader program object with vertex shader attributes:
		GLES32.glBindAttribLocation(ShaderProgramObject, GLESMacros.YSG_ATTRIBUTE_POSITION, "vPosition");

		// Pre-link binding of shader program object with fragment shader attributes:
		GLES32.glBindAttribLocation(ShaderProgramObject, GLESMacros.YSG_ATTRIBUTE_TEXTURE0, "vTexture0_Coord");
		
		// Link the two shaders together to shader program object to get a single executable and check for errors:
		GLES32.glLinkProgram(ShaderProgramObject);
		
		// Error checking:
		int[] iShaderProgramLinkStatus = new int[1];		// Linking check
		iInfoLogLength[0] = 0;								// Re initialize
		szInfoLog = null;									// Re initialize
		
		GLES32.glGetProgramiv(ShaderProgramObject, GLES32.GL_LINK_STATUS, iShaderProgramLinkStatus, 0);
		if(iShaderProgramLinkStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetProgramiv(ShaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			if(iInfoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetProgramInfoLog(ShaderProgramObject);
				System.out.println("YSG : Shader Program link log = "+szInfoLog);
				uninitialize();
				System.exit(0);
			}
		}
		
		// Get MVP Uniform location:
		// The actual locations assigned to uniform variables are not known until the program object is linked successfully.
		mvpUniform = GLES32.glGetUniformLocation(ShaderProgramObject, "u_mvp_matrix");
		
		// Get texture sampler uniform location:
		texture0_sampler_uniform = GLES32.glGetUniformLocation(ShaderProgramObject, "u_texture0_sampler");
		
		// Load textures:
		texture_stone[0] = loadGLTexture(R.raw.stone);
		texture_kundali[0] = loadGLTexture(R.raw.kundali_mirror);
		
		// Vertices, Colors, Shader attributes, vbo, vao initializations:
		final float pyramidVertices[] = new float[]
										{	// Perspective triangle (Front face)
											0.0f, 1.0f, 0.0f,		// Apex
											-1.0f, -1.0f, 1.0f,		// Left bottom
											1.0f, -1.0f, 1.0f,		// Right bottom
											// Perspective triangle (Right face)
											0.0f, 1.0f, 0.0f,		// Apex
											1.0f, -1.0f, 1.0f,		// Left bottom
											1.0f, -1.0f, -1.0f,		// Right bottom
											// Perspective triangle (Back face)
											0.0f, 1.0f, 0.0f,		// Apex
											1.0f, -1.0f, -1.0f,		// Left bottom
											-1.0f, -1.0f, -1.0f,	// Right bottom
											// Perspective triangle (Left face)
											0.0f, 1.0f, 0.0f,		// Apex
											-1.0f, -1.0f, -1.0f,		// Left bottom
											-1.0f, -1.0f, 1.0f		// Right bottom
										};

		final	float pyramidTexcoords[]= new float[]
										{
										0.5f, 1.0f, // front-top
										0.0f, 0.0f, // front-left
										1.0f, 0.0f, // front-right
            
										0.5f, 1.0f, // right-top
										1.0f, 0.0f, // right-left
										0.0f, 0.0f, // right-right
            
										0.5f, 1.0f, // back-top
										1.0f, 0.0f, // back-left
										0.0f, 0.0f, // back-right
            
										0.5f, 1.0f, // left-top
										0.0f, 0.0f, // left-left
										1.0f, 0.0f, // left-right
										};
										
		final float cubeVertices[] = new float[]
										{	// Perspective square (Top face)
											1.0f, 1.0f, -1.0f,		// Right top
											-1.0f, 1.0f, -1.0f, 	// Left top
											-1.0f, 1.0f, 1.0f,		// Left bottom
											1.0f, 1.0f, 1.0f,		// Right bottom
											// Perspective square (Bottom face)
											1.0f, -1.0f, -1.0f,		// Right top
											-1.0f, -1.0f, -1.0f, 	// Left top
											-1.0f, -1.0f, 1.0f,		// Left bottom
											1.0f, -1.0f, 1.0f,		// Right bottom
											// Perspective square (Front face)
											1.0f, 1.0f, 1.0f,		// Right top
											-1.0f, 1.0f, 1.0f,		// Left top
											-1.0f, -1.0f, 1.0f, 	// Left bottom
											1.0f, -1.0f, 1.0f,		// Right bottom
											// Perspective square (Back face)
											1.0f, 1.0f, -1.0f,		// Right top											
											-1.0f, 1.0f, -1.0f,		// Left top
											-1.0f, -1.0f, -1.0f, 	// Left bottom
											1.0f, -1.0f, -1.0f,		// Right bottom
											// Perspective square (Right face)
											1.0f, 1.0f, -1.0f,		// Right top											
											1.0f, 1.0f, 1.0f,		// Left top
											1.0f, -1.0f, 1.0f, 		// Left bottom
											1.0f, -1.0f, -1.0f,		// Right bottom
											// Perspective square (Left face)
											-1.0f, 1.0f, 1.0f,		// Right top																						
											-1.0f, 1.0f, -1.0f,		// Left top
											-1.0f, -1.0f, -1.0f, 	// Left bottom
											-1.0f, -1.0f, 1.0f		// Right bottom
										};
		
        final float cubeTexcoords[]= new float[]
										{
											0.0f,0.0f,
											1.0f,0.0f,
											1.0f,1.0f,
											0.0f,1.0f,
            
											0.0f,0.0f,
											1.0f,0.0f,
											1.0f,1.0f,
											0.0f,1.0f,
            
											0.0f,0.0f,
											1.0f,0.0f,
											1.0f,1.0f,
											0.0f,1.0f,
            
											0.0f,0.0f,
											1.0f,0.0f,
											1.0f,1.0f,
											0.0f,1.0f,
            
											0.0f,0.0f,
											1.0f,0.0f,
											1.0f,1.0f,
											0.0f,1.0f,
            
											0.0f,0.0f,
											1.0f,0.0f,
											1.0f,1.0f,
											0.0f,1.0f,
										};
										
		// A. BLOCK FOR PYRAMID:
		GLES32.glGenVertexArrays(1, vaoPyramid, 0);								// Generate vertex array object names (here, pyramid)
		GLES32.glBindVertexArray(vaoPyramid[0]);								// Bind a 'single' vertex array object
		
		// 1. BUFFER BLOCK FOR VERTICES:
		GLES32.glGenBuffers(1, vboPyramidPosition, 0);							// Generate buffer object names (here, for vertices);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboPyramidPosition[0]);		// Bind a named buffer object to the specified binding point
		
		// 5 steps to put data in a buffer:
		// 1. Create a ByteBuffer and allocate enough memory to it
		// 2. Tell java the byte order that you want to give to the buffer data
		// 3. Convert it to the data type that you want (eg float)
		// 4. Put data into the buffer
		// 5. Where to start from in the data
		ByteBuffer PyramidPosByteBuffer = ByteBuffer.allocateDirect(pyramidVertices.length * 4);
		PyramidPosByteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer PyramidVerticesBuffer = PyramidPosByteBuffer.asFloatBuffer();
		PyramidVerticesBuffer.put(pyramidVertices);
		PyramidVerticesBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 		// target
							pyramidVertices.length * 4,		// size in bytes of the buffer object's new data store
							PyramidVerticesBuffer,			// pointer to the data that will be copied into the data store
							GLES32.GL_STATIC_DRAW);			// expected usage pattern of the data store
		
		GLES32.glVertexAttribPointer(GLESMacros.YSG_ATTRIBUTE_POSITION,	// Index of the generic vertex attribute to be modified (vertex)
									3, 									// Number of components per generic vertex attribute (vertex)
									GLES32.GL_FLOAT,					// Data type of each component in the array
									false, 0, 0);						// normalized, stride, offset of the first component
		
		GLES32.glEnableVertexAttribArray(GLESMacros.YSG_ATTRIBUTE_POSITION);
		
		// Release the buffers for vertices
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		// 2. BUFFER BLOCK FOR TEXTURE:
		GLES32.glGenBuffers(1, vboPyramidTexture, 0);								// Generate buffer object names (here, for texture);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboPyramidTexture[0]);		// Bind a named buffer object to the specified binding point
		
		// 5 steps to put data in a buffer:
		// 1. Create a ByteBuffer and allocate enough memory to it
		// 2. Tell java the byte order that you want to give to the buffer data
		// 3. Convert it to the data type that you want (eg float)
		// 4. Put data into the buffer
		// 5. Where to start from in the data
		ByteBuffer PyramidTextureByteBuffer = ByteBuffer.allocateDirect(pyramidTexcoords.length * 4);
		PyramidTextureByteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer PyramidTextureBuffer = PyramidTextureByteBuffer.asFloatBuffer();
		PyramidTextureBuffer.put(pyramidTexcoords);
		PyramidTextureBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 		// target
							pyramidTexcoords.length * 4,	// size in bytes of the buffer object's new data store
							PyramidTextureBuffer,			// pointer to the data that will be copied into the data store
							GLES32.GL_STATIC_DRAW);			// expected usage pattern of the data store
		
		GLES32.glVertexAttribPointer(GLESMacros.YSG_ATTRIBUTE_TEXTURE0,	// Index of the generic vertex attribute to be modified (color)
									2, 									// Number of components per generic vertex attribute (color)
									GLES32.GL_FLOAT,					// Data type of each component in the array
									false, 0, 0);						// normalized, stride, offset of the first component
		
		GLES32.glEnableVertexAttribArray(GLESMacros.YSG_ATTRIBUTE_TEXTURE0);
		
		// Release the buffer for colors:
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		// Release vao:
		GLES32.glBindVertexArray(0);
		
		// B. BLOCK FOR CUBE:
		GLES32.glGenVertexArrays(1, vaoCube, 0);					// Generate vertex array object names (here, Cube)
		GLES32.glBindVertexArray(vaoCube[0]);						// Bind a 'single' vertex array object
		
		// 1.. BUFFER BLOCK FOR VERTICES:
		GLES32.glGenBuffers(1, vboCubePosition, 0);								// Generate buffer object names (here, for vertices);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboCubePosition[0]);		// Bind a named buffer object to the specified binding point
		
		// 5 steps to put data in a buffer:
		// 1. Create a ByteBuffer and allocate enough memory to it
		// 2. Tell java the byte order that you want to give to the buffer data
		// 3. Convert it to the data type that you want (eg float)
		// 4. Put data into the buffer
		// 5. Where to start from in the data
		ByteBuffer CubePosByteBuffer = ByteBuffer.allocateDirect(cubeVertices.length * 4);
		CubePosByteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer CubeVerticesBuffer = CubePosByteBuffer.asFloatBuffer();
		CubeVerticesBuffer.put(cubeVertices);
		CubeVerticesBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 		// target
							cubeVertices.length * 4,		// size in bytes of the buffer object's new data store
							CubeVerticesBuffer,				// pointer to the data that will be copied into the data store
							GLES32.GL_STATIC_DRAW);			// expected usage pattern of the data store
		
		GLES32.glVertexAttribPointer(GLESMacros.YSG_ATTRIBUTE_POSITION,	// Index of the generic vertex attribute to be modified (vertex)
									3, 									// Number of components per generic vertex attribute (vertex)
									GLES32.GL_FLOAT,					// Data type of each component in the array
									false, 0, 0);						// normalized, stride, offset of the first component
		
		GLES32.glEnableVertexAttribArray(GLESMacros.YSG_ATTRIBUTE_POSITION);
		
		// Release the buffers for vertices
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		
		// 2. BUFFER BLOCK FOR TEXTURE:
		GLES32.glGenBuffers(1, vboCubeTexture, 0);								// Generate buffer object names (here, for texture);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboCubeTexture[0]);			// Bind a named buffer object to the specified binding point
		
		// 5 steps to put data in a buffer:
		// 1. Create a ByteBuffer and allocate enough memory to it
		// 2. Tell java the byte order that you want to give to the buffer data
		// 3. Convert it to the data type that you want (eg float)
		// 4. Put data into the buffer
		// 5. Where to start from in the data
		ByteBuffer CubeTextureByteBuffer = ByteBuffer.allocateDirect(cubeTexcoords.length * 4);
		CubeTextureByteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer CubeTextureBuffer = CubeTextureByteBuffer.asFloatBuffer();
		CubeTextureBuffer.put(cubeTexcoords);
		CubeTextureBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 		// target
							cubeTexcoords.length * 4,		// size in bytes of the buffer object's new data store
							CubeTextureBuffer,				// pointer to the data that will be copied into the data store
							GLES32.GL_STATIC_DRAW);			// expected usage pattern of the data store
		
		GLES32.glVertexAttribPointer(GLESMacros.YSG_ATTRIBUTE_TEXTURE0,	// Index of the generic vertex attribute to be modified (color)
									2, 									// Number of components per generic vertex attribute (color)
									GLES32.GL_FLOAT,					// Data type of each component in the array
									false, 0, 0);						// normalized, stride, offset of the first component
		
		GLES32.glEnableVertexAttribArray(GLESMacros.YSG_ATTRIBUTE_TEXTURE0);
		
		// Release the buffer for texture:
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		// Release vao:
		GLES32.glBindVertexArray(0);
		
		// Enable depth testing and specify the depth test to perform:
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		
		// We will disable culling of back faces for 3D rotations as well:
		//GLES32.glEnable(GLES32.GL_CULL_FACE);
		
		// Set the background color to Black:
		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		
		// Set projectionMatrix and Identity matrix:
		Matrix.setIdentityM(PerspectiveProjectionMatrix, 0);
	}

    private int loadGLTexture(int imageFileResourceID)
    {
        // Code:
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inScaled = false;
        
        // Read in the resource
        Bitmap bitmap = BitmapFactory.decodeResource(myContext.getResources(), imageFileResourceID, options);
        
        int[] texture=new int[1];
        
        // Create a texture object to apply to model:
        GLES32.glGenTextures(1, texture, 0);
        
        // Indicate that pixel rows are tightly packed (defaults to stride of 4 which is kind of only good for RGBA or FLOAT data types):
        GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT,1);
        
        // Bind with the texture
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D,texture[0]);
        
        // Set up filter and wrap modes for this texture object:
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D,GLES32.GL_TEXTURE_MAG_FILTER,GLES32.GL_LINEAR);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D,GLES32.GL_TEXTURE_MIN_FILTER,GLES32.GL_LINEAR_MIPMAP_LINEAR);
        
        // Load the bitmap into the bound texture:
        GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0);
        
        // Generate mipmap:
        GLES32.glGenerateMipmap(GLES32.GL_TEXTURE_2D);
        
        return(texture[0]);
    }
	
	private void resize(int width, int height)
	{
		//Code:
		// Set the viewport:
		GLES32.glViewport(0, 0, width, height);
		
		// Perspective Projection = FOV, aspect ratio, near, far:
		Matrix.perspectiveM(PerspectiveProjectionMatrix, 0, 45.0f, 
							((float)width/(float)height), 0.1f, 100.0f);
	}
	
	public void display()
	{
		// Code:
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
		
		// Use shader program:
		GLES32.glUseProgram(ShaderProgramObject);
		
		// OpenGL ES drawing:
		float ModelViewMatrix[] = new float[16];				// Eye space 
		float ModelViewProjectionMatrix[] = new float[16];		// Clip space
		float RotationMatrix[] = new float[16];					// Rotation matrix
		float ScaleMatrix[] = new float[16];					// Scaling matrix
		
		// A. DRAW PYRAMID:
		// Set modelview, modelviewprojection and Rotation matrices to the identity matrix
		Matrix.setIdentityM(ModelViewMatrix, 0);
		Matrix.setIdentityM(ModelViewProjectionMatrix, 0);
		Matrix.setIdentityM(RotationMatrix, 0);
		
		// 1. Translation:
		// Translate the ModelViewMatrix and store back in the ModelViewMatrix:
		Matrix.translateM(ModelViewMatrix, 0, ModelViewMatrix, 0, -1.5f, 0.0f, -6.0f);			// Translate the Pyramid to the left
		
		// 2. Rotation:
		// Rotate the Rotation Matrix
		Matrix.rotateM(RotationMatrix, 0, RotationMatrix, 0, anglePyramid, 0.0f, 1.0f, 0.0f);	// Rotate Pyramid about the y axis
		
		// Multiply ModelViewMatrix and RotationMatrix and store back in ModelViewMatrix:
		Matrix.multiplyMM(ModelViewMatrix, 0,
						ModelViewMatrix, 0,
						RotationMatrix, 0);
						
		// Multiply the modelview and projection matrix to get the modelviewprojection matrix:
		Matrix.multiplyMM(ModelViewProjectionMatrix, 0,
						PerspectiveProjectionMatrix, 0,
						ModelViewMatrix, 0);
						
		// Pass the above modelviewprojection matrix to the vertex shader in "u_mvp_matrix" shader variable, 
		// whose position we have already calculated in Initialize() by using glGetUniformLocation():
		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, ModelViewProjectionMatrix, 0);
		
		// Bind vao:
		GLES32.glBindVertexArray(vaoPyramid[0]);
		
		// 3 steps:
		// Bind with pyramid texture:
		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture_stone[0]);
		GLES32.glUniform1i(texture0_sampler_uniform, 0); // 0th sampler enable (as we have only 1 texture sampler in fragment shader)
		
		// Draw either using glDrawTriangles() or glDrawArrays() or glDrawElements():
		GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 12);		// 3 (each with x,y,z) vertices in pyramidVertices array
		
		// Unbind vao:
		GLES32.glBindVertexArray(0);

		// B. DRAW CUBE:
		// Set modelview, modelviewprojection, scale and rotation matrices to the identity matrix
		Matrix.setIdentityM(ModelViewMatrix, 0);
		Matrix.setIdentityM(ModelViewProjectionMatrix, 0);
		Matrix.setIdentityM(RotationMatrix, 0);
		Matrix.setIdentityM(ScaleMatrix, 0);
		
		// 1. Scaling:
		// Scale the ScaleMatrix and store back in the ScaleMatrix:
		Matrix.scaleM(ScaleMatrix, 0, ScaleMatrix, 0, 0.75f, 0.75f, 0.75f);					// Scale the cube by 0.75 on all sides
		
		// 2. Translation:
		// Translate the ModelViewMatrix and store back in the ModelViewMatrix:
		Matrix.translateM(ModelViewMatrix, 0, ModelViewMatrix, 0, 1.5f, 0.0f, -6.0f);		// Translate the cube to the right

		// Multiply ModelViewMatrix and ScaleMatrix and store back in ModelViewMatrix:
		Matrix.multiplyMM(ModelViewMatrix, 0,
						ModelViewMatrix, 0,
						ScaleMatrix, 0);
						
		// 3. Rotation:
		// Rotate the Rotation Matrix
		Matrix.rotateM(RotationMatrix, 0, RotationMatrix, 0, angleCube, 1.0f, 0.0f, 0.0f);	// Rotate cube about the x axis
		Matrix.rotateM(RotationMatrix, 0, RotationMatrix, 0, angleCube, 0.0f, 1.0f, 0.0f);	// Rotate cube about the y axis
		Matrix.rotateM(RotationMatrix, 0, RotationMatrix, 0, angleCube, 0.0f, 0.0f, 1.0f);	// Rotate cube about the z axis
		
		// Multiply ModelViewMatrix and RotationMatrix and store back in ModelViewMatrix:
		Matrix.multiplyMM(ModelViewMatrix, 0,
						ModelViewMatrix, 0,
						RotationMatrix, 0);
						
		// Multiply the modelview and projection matrix to get the modelviewprojection matrix:
		Matrix.multiplyMM(ModelViewProjectionMatrix, 0,
						PerspectiveProjectionMatrix, 0,
						ModelViewMatrix, 0);
						
		// Pass the above modelviewprojection matrix to the vertex shader in "u_mvp_matrix" shader variable, 
		// whose position we have already calculated in Initialize() by using glGetUniformLocation():
		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, ModelViewProjectionMatrix, 0);
		
		// Bind vao:
		GLES32.glBindVertexArray(vaoCube[0]);
		
		// 3 steps:
		// Bind with cube texture:
        GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D,texture_kundali[0]);
        GLES32.glUniform1i(texture0_sampler_uniform, 0);		// 0th sampler enable ( as we have only 1 texture sampler in fragment shader )
		
		// Draw either using glDrawTriangles() or glDrawArrays() or glDrawElements():
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);		// 4 for the triangle fan to draw a quad
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 4, 4);		// 4 for the triangle fan to draw a quad
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 8, 4);		// 4 for the triangle fan to draw a quad
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 12, 4);		// 4 for the triangle fan to draw a quad
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 16, 4);		// 4 for the triangle fan to draw a quad
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 20, 4);		// 4 for the triangle fan to draw a quad
		
		// Unbind vao:
		GLES32.glBindVertexArray(0);
		
		// Stop using the shader program object:
		GLES32.glUseProgram(0);
		
		// Update for rotation:
		update();
		
		// Render/Flush:
		requestRender();
	}
	
	// Update function:
	private void update()
	{
		anglePyramid = anglePyramid + 1.0f;
		if (anglePyramid >= 360.0f)
			anglePyramid = 0.0f;

		angleCube = angleCube - 1.0f;
		if (angleCube <= -360.0f)
			angleCube = 0.0f;			
	}
	
	void uninitialize()
	{
		// Code:
		// Destroy vao for Pyramid:
		if(vaoPyramid[0] != 0)
		{
			GLES32.glDeleteVertexArrays(1, vaoPyramid, 0);
			vaoPyramid[0] = 0;
		}

		// Destroy vao for Cube:
		if(vaoCube[0] != 0)
		{
			GLES32.glDeleteVertexArrays(1, vaoCube, 0);
			vaoCube[0] = 0;
		}
		
		// Destroy vbo for Pyramid position:
		if(vboPyramidPosition[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vboPyramidPosition, 0);
			vboPyramidPosition[0] = 0;
		}

		// Destroy vbo for Pyramid texture:
		if(vboPyramidTexture[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vboPyramidTexture, 0);
			vboPyramidTexture[0] = 0;
		}
		
		// Destroy vbo for Cube position:
		if(vboCubePosition[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vboCubePosition, 0);
			vboCubePosition[0] = 0;
		}

		// Destroy vbo for Cube texture:
		if(vboCubeTexture[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vboCubeTexture, 0);
			vboCubeTexture[0] = 0;
		}
		
		// Delete texture objects:
        if (texture_stone[0] != 0)
        {
            GLES32.glDeleteTextures(1, texture_stone, 0);
            texture_stone[0] = 0;
        }
        if (texture_kundali[0] != 0)
        {
            GLES32.glDeleteTextures(1, texture_kundali, 0);
            texture_kundali[0] = 0;
        }
		
		// Detach and delete the shaders one by one that are attached to the the shader program object
		if(ShaderProgramObject != 0)
		{
			if(VertexShaderObject != 0)
			{
				// Detach vertex shader from the shader program and then delete it:
				GLES32.glDetachShader(ShaderProgramObject, VertexShaderObject);
				GLES32.glDeleteShader(VertexShaderObject);
				VertexShaderObject = 0;
			}
			
			if(FragmentShaderObject != 0)
			{
				// Detach fragment shader from the shader program and then delete it:
				GLES32.glDetachShader(ShaderProgramObject, FragmentShaderObject);
				GLES32.glDeleteShader(FragmentShaderObject);
				FragmentShaderObject = 0;
			}
		}
		
		// Delete the shader program object
		if(ShaderProgramObject != 0)
		{
			GLES32.glDeleteProgram(ShaderProgramObject);
			ShaderProgramObject = 0;
		}
	}
}
		