package com.yashg.checkerboard_texture;							// For procedural checkerboard texture

import android.content.Context;									// For "Context" class

import android.opengl.GLSurfaceView;							// For "GLSurfaceView" class
import android.opengl.GLES32;									// For OpenGL ES 32
import android.opengl.GLES20;									// For OpenGL ES 20

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
	
	// Checker board variables:
	static final int checkImageWidth = 64;
    static final int checkImageHeight = 64;

	// vao and vbo:
	private int[] vaoQuad = new int[1];
	private int[] vboQuadPosition = new int[1];
	private int[] vboQuadTexture = new int[1];
	
	private int mvpUniform;
	private int texture0_sampler_uniform;			// Texture sampler
	
	// Texture objects:
	private int[] texture = new int[1];

	private float PerspectiveProjectionMatrix[] = new float[16];		// 4x4 matrix
	
	// Quad vertices:
	private float quadVertices[] = new float[12];
    private float quadVertices2[] = new float[12];

	// Multidimensional array for the checkerboard:
    private byte[][][] checkImage = new byte[checkImageWidth][checkImageHeight][4];
    private byte[] array_buffer = new byte[16384];
	private int n = 0;
	
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
		
		// Vertices, Colors, Shader attributes, vbo, vao initializations:
		final float quadVertices[] = new float[]
										{	
											-1.0f, 1.0f, 0.0f,
											1.0f, 1.0f, 0.0f,
											1.0f, -1.0f, 0.0f,
											-1.0f, -1.0f, 0.0f
										};

		final float quadTexcoords[]= new float[]
										{
										    1.0f, 1.0f,
											0.0f, 1.0f,
											0.0f, 0.0f,
											1.0f, 0.0f
										};
										
		// A. VAO for QUAD:
		GLES32.glGenVertexArrays(1, vaoQuad, 0);		// Generate vertex array object names (here, pyramid)
		GLES32.glBindVertexArray(vaoQuad[0]);			// Bind a 'single' vertex array object
		
		// 1. BUFFER BLOCK FOR VERTICES:
		GLES32.glGenBuffers(1, vboQuadPosition, 0);							// Generate buffer object names (here, for vertices);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboQuadPosition[0]);	// Bind a named buffer object to the specified binding point
		
		// 5 steps to put data in a buffer:
		// 1. Create a ByteBuffer and allocate enough memory to it
		// 2. Tell java the byte order that you want to give to the buffer data
		// 3. Convert it to the data type that you want (eg float)
		// 4. Put data into the buffer
		// 5. Where to start from in the data
		/*
		ByteBuffer PyramidPosByteBuffer = ByteBuffer.allocateDirect(pyramidVertices.length * 4);
		PyramidPosByteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer PyramidVerticesBuffer = PyramidPosByteBuffer.asFloatBuffer();
		PyramidVerticesBuffer.put(pyramidVertices);
		PyramidVerticesBuffer.position(0);
		*/
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 		// target
							48,				// size in bytes of the buffer object's new data store
							null,			// pointer to the data that will be copied into the data store
							GLES32.GL_DYNAMIC_DRAW);			// expected usage pattern of the data store
		
		GLES32.glVertexAttribPointer(GLESMacros.YSG_ATTRIBUTE_POSITION,	// Index of the generic vertex attribute to be modified (vertex)
									3, 									// Number of components per generic vertex attribute (vertex)
									GLES32.GL_FLOAT,					// Data type of each component in the array
									false, 0, 0);						// normalized, stride, offset of the first component
		
		GLES32.glEnableVertexAttribArray(GLESMacros.YSG_ATTRIBUTE_POSITION);
		
		// Release the buffers for vertices
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		// 2. BUFFER BLOCK FOR TEXTURE:
		GLES32.glGenBuffers(1, vboQuadTexture, 0);							// Generate buffer object names (here, for texture);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboQuadTexture[0]);		// Bind a named buffer object to the specified binding point
		
		// 5 steps to put data in a buffer:
		// 1. Create a ByteBuffer and allocate enough memory to it
		// 2. Tell java the byte order that you want to give to the buffer data
		// 3. Convert it to the data type that you want (eg float)
		// 4. Put data into the buffer
		// 5. Where to start from in the data
		ByteBuffer QuadTextureByteBuffer = ByteBuffer.allocateDirect(quadTexcoords.length * 4);
		QuadTextureByteBuffer.order(ByteOrder.nativeOrder());
		FloatBuffer QuadTextureBuffer = QuadTextureByteBuffer.asFloatBuffer();
		QuadTextureBuffer.put(quadTexcoords);
		QuadTextureBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 		// target
							quadTexcoords.length * 4,	// size in bytes of the buffer object's new data store
							QuadTextureBuffer,			// pointer to the data that will be copied into the data store
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
		
		LoadProceduralTexture();
		
		// Set projectionMatrix and Identity matrix:
		Matrix.setIdentityM(PerspectiveProjectionMatrix, 0);
	}
	
	private void LoadProceduralTexture()
    {

    	ByteBuffer byteArrayBuffer;
        MakeCheckImage();
        arrayGen();

        byteArrayBuffer = ByteBuffer.allocateDirect(array_buffer.length * 4);
		byteArrayBuffer.order(ByteOrder.nativeOrder());
		byteArrayBuffer.put(array_buffer);
		byteArrayBuffer.position(0);

        GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 1);
        GLES32.glGenTextures(1, texture, 0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture[0]);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_S, GLES32.GL_REPEAT);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_T, GLES32.GL_REPEAT);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_NEAREST);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_NEAREST);

        GLES32.glTexImage2D(GLES32.GL_TEXTURE_2D, 0, GLES32.GL_RGBA, checkImageWidth, checkImageHeight, 0, GLES32.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, byteArrayBuffer);

        //GLES10.glTexEnvf(GLES10.GL_TEXTURE_ENV, GLES10.GL_TEXTURE_ENV_MODE, GLES10.GL_REPLACE);
    }

    private void MakeCheckImage()
    {
        int i, j, c = 0;

        boolean firstop, secondop;

        for (i = 0; i < checkImageHeight; i++)
        {
            for (j = 0; j < checkImageWidth; j++)
            {

            	firstop = ((i & 0x8) == 0);
            	secondop = ((j & 0x8) == 0);

            	int first = firstop ? 0 : 1;
				int second = secondop ? 0 : 1;

                c = (first ^ second) * 255;

                checkImage[i][j][0] = (byte)c;
                checkImage[i][j][1] = (byte)c;
                checkImage[i][j][2] = (byte)c;
                checkImage[i][j][3] = (byte)255;

            }
        }
    }

	public void arrayGen()
	{
		System.out.println("OGLES: In arrayGen");
		for(int i = 0; i < 64; i++)
		{
			for(int j = 0; j < 64; j++)
			{
				for(int k = 0; k< 4; k++)
				{
					array_buffer[n++] = checkImage[i][j][k];
				}
			//n = n + 1;
			}
		}	
	}

	
	/*
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
	*/
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
		
		// A. DRAW QUAD:
		// Set modelview, modelviewprojection and Rotation matrices to the identity matrix
		Matrix.setIdentityM(ModelViewMatrix, 0);
		Matrix.setIdentityM(ModelViewProjectionMatrix, 0);
		
		// 1. Translation:
		// Translate the ModelViewMatrix and store back in the ModelViewMatrix:
		Matrix.translateM(ModelViewMatrix, 0, ModelViewMatrix, 0, 0.0f, 0.0f, -5.0f);			// Translate the Pyramid to the left
						
		// Multiply the modelview and projection matrix to get the modelviewprojection matrix:
		Matrix.multiplyMM(ModelViewProjectionMatrix, 0,
						PerspectiveProjectionMatrix, 0,
						ModelViewMatrix, 0);
						
		// Pass the above modelviewprojection matrix to the vertex shader in "u_mvp_matrix" shader variable, 
		// whose position we have already calculated in Initialize() by using glGetUniformLocation():
		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, ModelViewProjectionMatrix, 0);
		
		// Bind vao:
		GLES32.glBindVertexArray(vaoQuad[0]);
		
		// 3 steps:
		// Bind with pyramid texture:
		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture[0]);
		GLES32.glUniform1i(texture0_sampler_uniform, 0); // 0th sampler enable (as we have only 1 texture sampler in fragment shader)
		
		quadVertices[0] = 0.0f;
        quadVertices[1] = 1.0f;
        quadVertices[2] = 0.0f;
        quadVertices[3] = -2.0f;
        quadVertices[4] = 1.0f;
        quadVertices[5] = 0.0f;
        quadVertices[6] = -2.0f;
        quadVertices[7] = -1.0f;
        quadVertices[8] = 0.0f;
        quadVertices[9] = 0.0f;
        quadVertices[10] = -1.0f;
        quadVertices[11] = 0.0f;

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboQuadPosition[0]);
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(quadVertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer verticesBuffer = byteBuffer.asFloatBuffer();
        verticesBuffer.put(quadVertices);
        verticesBuffer.position(0);  

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, quadVertices.length * 4, verticesBuffer, GLES32.GL_DYNAMIC_DRAW);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		// Draw either using glDrawTriangles() or glDrawArrays() or glDrawElements():
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);		// 3 (each with x,y,z) vertices in pyramidVertices array
		
		// Unbind vao:
		GLES32.glBindVertexArray(0);

		// Set modelview, modelviewprojection, scale and rotation matrices to the identity matrix
		Matrix.setIdentityM(ModelViewMatrix, 0);
		Matrix.setIdentityM(ModelViewProjectionMatrix, 0);
			
		// 1. Translation:
		// Translate the ModelViewMatrix and store back in the ModelViewMatrix:
		Matrix.translateM(ModelViewMatrix, 0, ModelViewMatrix, 0, 0.0f, 0.0f, -5.0f);		// Translate the cube to the right
				
		// Multiply the modelview and projection matrix to get the modelviewprojection matrix:
		Matrix.multiplyMM(ModelViewProjectionMatrix, 0,
						PerspectiveProjectionMatrix, 0,
						ModelViewMatrix, 0);
						
		// Pass the above modelviewprojection matrix to the vertex shader in "u_mvp_matrix" shader variable, 
		// whose position we have already calculated in Initialize() by using glGetUniformLocation():
		GLES32.glUniformMatrix4fv(mvpUniform, 1, false, ModelViewProjectionMatrix, 0);
		
		// Bind vao:
		GLES32.glBindVertexArray(vaoQuad[0]);
		
		// 3 steps:
		// Bind with cube texture:
        GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D,texture[0]);
        GLES32.glUniform1i(texture0_sampler_uniform, 0);		// 0th sampler enable ( as we have only 1 texture sampler in fragment shader )
		
		quadVertices2[0] = 2.41421f;
        quadVertices2[1] = 1.0f;
        quadVertices2[2] = -1.41421f;
        quadVertices2[3] = 1.0f;
        quadVertices2[4] = 1.0f;
        quadVertices2[5] = 0.0f;
        quadVertices2[6] = 1.0f;
        quadVertices2[7] = -1.0f;
        quadVertices2[8] = 0.0f;
        quadVertices2[9] = 2.41421f;
        quadVertices2[10] = -1.0f;
        quadVertices2[11] = -1.41421f;

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboQuadPosition[0]);
        byteBuffer = ByteBuffer.allocateDirect(quadVertices2.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        verticesBuffer = byteBuffer.asFloatBuffer();
        verticesBuffer.put(quadVertices2);
        verticesBuffer.position(0);  
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, quadVertices2.length * 4, verticesBuffer, GLES32.GL_DYNAMIC_DRAW);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

		// Draw either using glDrawTriangles() or glDrawArrays() or glDrawElements():
		GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);		// 4 for the triangle fan to draw a quad
		
		// Unbind vao:
		GLES32.glBindVertexArray(0);
		
		// Stop using the shader program object:
		GLES32.glUseProgram(0);
		
		// Render/Flush:
		requestRender();
	}
	
	void uninitialize()
	{
		// Code:
		// Destroy vao for Quad:
		if(vaoQuad[0] != 0)
		{
			GLES32.glDeleteVertexArrays(1, vaoQuad, 0);
			vaoQuad[0] = 0;
		}

		// Destroy vbo for Quad position:
		if(vboQuadPosition[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vboQuadPosition, 0);
			vboQuadPosition[0] = 0;
		}

		// Destroy vbo for Quad texture:
		if(vboQuadTexture[0] != 0)
		{
			GLES32.glDeleteBuffers(1, vboQuadTexture, 0);
			vboQuadTexture[0] = 0;
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
		