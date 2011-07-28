/*******************notes***************/


#define for if(1) for

#include <iostream>
#include <assert.h>
#include <vector>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include "window.h"


#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>

#else

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <X11/Xlib.h>

#endif

#include "common.h"
#include "ludecomp.h"
#include "stopwatch.hpp"

#include "shaderprog.h"

#include "programs.h"

using namespace std;

LUDecomp * lu;
Window hWindow;
Display *hDisplay;

void lugpu_initilize(int argc, char ** argv)   // not changing this, assuming glutES is identical
{
        /*glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(1,1);
	glutCreateWindow("LUGPU");*/

	EGLDisplay	sEGLDisplay;
	EGLContext	sEGLContext;
	EGLSurface	sEGLSurface;
	const unsigned int uiWidth  = 640;
	const unsigned int uiHeight = 480;
	
	/* EGL Configuration */
	
	EGLint aEGLAttributes[] = {
		EGL_RED_SIZE, 0,
		EGL_GREEN_SIZE, 0,
		EGL_BLUE_SIZE, 0,
		EGL_LUMINANCE_SIZE, 32,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};
	
	EGLint aEGLContextAttributes[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};
	
	EGLConfig	aEGLConfigs[1];
	EGLint		cEGLConfigs;

	XSetWindowAttributes win_attrs;
	int attrs[64], idx = 0, num_config = 0;
	int major, minor;
 	Colormap colormap;
    	XVisualInfo *pVisual;
    	XEvent e;

	/*hDisplay = XOpenDisplay(NULL);

	if (!hDisplay) {
	  printf("Could not open display\n");
	  exit(-1);
	  }*/
	
	sEGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	//sEGLDisplay = EGL_CHECK(eglGetDisplay((EGLNativeDisplayType)hDisplay));
	
	eglInitialize(sEGLDisplay, NULL, NULL);
	EGL_CHECK(eglChooseConfig(sEGLDisplay, aEGLAttributes, aEGLConfigs, 1, &cEGLConfigs));
	fprintf(stderr, "chooseConfig: %i", eglGetError());
	if (cEGLConfigs == 0) {
        printf("No EGL configurations were returned.\n");
		exit(-1);
    }

	hWindow = create_window("OpenGL ES 2.0 Example on a Linux Desktop", uiWidth,
				uiHeight, hDisplay, sEGLDisplay, aEGLConfigs[0], &colormap, &pVisual);

	sEGLSurface = EGL_CHECK(eglCreateWindowSurface(sEGLDisplay, aEGLConfigs[0], (EGLNativeWindowType)hWindow, NULL));

	if (sEGLSurface == EGL_NO_SURFACE) {
	  printf("Failed to create EGL surface.\n");
	  exit(-1);
	}
	
 	sEGLContext = EGL_CHECK(eglCreateContext(sEGLDisplay, aEGLConfigs[0], EGL_NO_CONTEXT, aEGLContextAttributes));

	if (sEGLContext == EGL_NO_CONTEXT) {
	  printf("Failed to create EGL context.\n");
	  exit(-1);
	}
	
	EGL_CHECK(eglMakeCurrent(sEGLDisplay, sEGLSurface, sEGLSurface, sEGLContext));
	LUDecomp::_CheckForGLError("Window Created");
	lu = new LUDecomp();
	lu->Initialize(400, 400, 1,0,0);

	fprintf(stderr, "extensions supported: %s",(char* )glGetString(GL_EXTENSIONS));
	
}



void lugpu_sgetrf(const int *m,const int *n,float *matrix,const int *lda,int *pivot,int *info) //leaving alone for now, not sure what does
{

	*info = 0;
	if(*m < 0)
		*info = -1;
	if(*n < 0)
		*info = -2;
	if(*lda < max(1,*m))
		*info = -4;

	if(*info != 0)
	{
		/****TODO*****/
		//call XERBLA
		return;
	}

	lu->SetSize(*m,*n);
	lu->LoadMatrix(matrix);

	lu->Compute();
	
	lu->GetMatrix(matrix);
}


//----------------------------------------------------------------------------
// Function     	: LUDecomp::LUDecomp
// Description	    : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::LUDecomp()
 * @brief Default Constructor
 */ 
LUDecomp::LUDecomp() 
: _xRes(1), _yRes(1), _n(0), _m(0), _ncomponents(1), _currentDrawSurface(0),
  _bInitialized(false), _bComputed(false), current01(1)
{
}


/**************BEGINNING CONVERSION TO OPENGLES********************/

void LUDecomp::SetSize(int m,int n)  //converted
{

	_m = m;
	_n = n;

	glActiveTexture( GL_TEXTURE0 );  
	glBindTexture(GL_TEXTURE_2D, textureid[0]);   //changed GL_TEXTURE_RECTANGE to 2D
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, _m, _n, 0, GL_LUMINANCE, GL_FLOAT, NULL);

	_CheckForGLError("Create Texture0");

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture(GL_TEXTURE_2D, textureid[1]);  
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, _m, _n, 0, GL_LUMINANCE, GL_FLOAT, NULL);

	_CheckForGLError("Create Texture1");
	
	glBindFramebuffer(GL_FRAMEBUFFER, fb[0]);

	glFramebufferTexture2D(GL_FRAMEBUFFER,
                                  GL_COLOR_ATTACHMENT0,
                                  GL_TEXTURE_2D, textureid[0], 0);

	_CheckForGLError("Bind Texture0");
	
	glBindFramebuffer(GL_FRAMEBUFFER, fb[1]);

	glFramebufferTexture2D(GL_FRAMEBUFFER,
                                  GL_COLOR_ATTACHMENT0,
                                  GL_TEXTURE_2D, textureid[1], 0);

	_CheckForGLError("Bind Texture1");

	glBindFramebuffer(GL_FRAMEBUFFER, fb[0]);

	glDepthMask(GL_FALSE);
	glClearColor(0, 0, 0, 0);
	glViewport(0, 0, _m, _n);
	_CheckForGLError("SetSize complete");

}

//----------------------------------------------------------------------------
// Function     	: LUDecomp::~LUDecomp
// Description	    : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::~LUDecomp()
 * @brief Destructor
 */ 
LUDecomp::~LUDecomp()
{

}

//----------------------------------------------------------------------------
// Function     	: LUDecomp::Initialize
// Description	    : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::Initialize()
 * @brief Initializes the LUDecomp computation.
 * 
 * Here we initialize and allocate necessary GPU resources.
 */ 
void LUDecomp::Initialize(int m, int n, int ncomp, int xblocksize, int yblocksize) //converted
{
	lu->_InitializeSlabOps();
	
	_m = m / ncomp;
	_n = n;
	_ncomponents = ncomp;
	assert(_ncomponents == 1 || _ncomponents == 4);

	_xRes = (xblocksize) ? (float)xblocksize / _n : 1.0;
	_yRes = (yblocksize) ? (float)yblocksize / _m : 1.0;

	_bInitialized = true;

	glGenFramebuffers(2, fb);	

	glGenTextures(1, &textureid[0]);
	glGenTextures(1, &textureid[1]);
	

	SetSize(_m,_n);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	_CheckForGLError("Init complete");
}

//----------------------------------------------------------------------------
// Function     	: LUDecomp::_InitializeSlabOps
// Description	    : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::InitializeSlabOps()
 * @brief Initializes the SlabOps used in the matrix multiplication 
 * 
 * Here we initialize the slab operations, passing them parameters such
 * as input textures, and fragment programs.
 */ 
void LUDecomp::_InitializeSlabOps()
{

  max_fp.Load(vertex_shader, max_op);
  swapcol_fp.Load(vertex_shader, swapcol_op);
  swaprow_fp.Load(vertex_shader, swaprow_op);
  divide_fp.Load(vertex_shader, divide_op);
  copy_fp.Load(vertex_shader, copy_op);
  quadtree_fp.Load(vertex_shader, quadtree_op);
  row_fp.Load(vertex_shader, row_op);
  _CheckForGLError("Init SlabOps");
}

//----------------------------------------------------------------------------
// Function     	: LUDecomp::Shutdown
// Description	    : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::Shutdown()
 * @brief Shuts down the LUDecomp simulator.
 */ 
void LUDecomp::Shutdown()
{
  
 }
//---------------------------------------------------------------------------
//Function             : LUDecomp::Draw
//Draws
//---------------------------------------------------------------------------
/** 
 * @fn LUDecomp::Draw()
 * @brief Draws matrix
 */
void LUDecomp::Draw(ShaderProg* program, GLfloat* vertices, GLfloat* texcoord0, GLfloat* texcoord1, int n)
{
  GLint posindex = 10;
  GLint texindex0 = 11;
  GLint texindex1 = 12;
  GLint texloc;
  GLubyte i[] = {0,1,2,   //first triangle (BL, TL ,TR)
		 0,2,3};  //second traingle(BL, TR, BR)
  _CheckForGLError("Draw Pre-Bind");
  program -> Bind();
  _CheckForGLError("Draw Post-Bind");
  
  
  texloc = glGetUniformLocation( program -> prog_id, "texture0");
  glUniform1i ( texloc, 0);

  posindex = glGetAttribLocation ( program -> prog_id, "position");
  texindex0 = glGetAttribLocation( program -> prog_id, "v_texcoord0");
  texindex1 = glGetAttribLocation( program -> prog_id, "v_texcoord1");

  glVertexAttribPointer ( posindex , 2 , GL_FLOAT, GL_FALSE, 0, vertices );
  glVertexAttribPointer ( texindex0, 2 , GL_FLOAT, GL_FALSE, 0, texcoord0);

  if(texindex1 != -1) {
    glVertexAttribPointer ( texindex1, 2 , GL_FLOAT, GL_FALSE, 0, texcoord1);
    glEnableVertexAttribArray ( texindex1 );
  }

  glEnableVertexAttribArray ( posindex );
  glEnableVertexAttribArray ( texindex0 );
  
  
  if(n==8) {
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, i );
  }
  else if(n==4) {
    glDrawArrays( GL_LINES, 0, 4);
  }
  else {
    glDrawArrays( GL_TRIANGLES, 0 , n);
  }
}

//----------------------------------------------------------------------------
// Function     	: LUDecomp::CopyRect
// Description	    : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::CopyRect()
 * @brief Copies a rectangle
 * 
 * 
 */ 
  void LUDecomp::CopyRect(ShaderProg* program,float xmin,float ymin,float xmax,float ymax,GLfloat* t)  //converted
{
	
	GLfloat v[8];
	v[0] = xmin;  v[1] = ymin;
	v[2] = xmin;  v[3] = ymax;
	v[4] = xmax;  v[5] = ymax;
	v[6] = xmax;  v[7] = ymin;

	if (t)
		Draw(program, v, t, t, 8);
	else
		Draw(program, v, v, v, 8);

	
}

//----------------------------------------------------------------------------
// Function     	: LUDecomp::Divide
// Description	    : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::Divide()
 * @brief Divides a rectangle by a value
 * 
 * 
 */ 
void LUDecomp::Divide(int k,float xmin,float ymin,float xmax,float ymax) //converting *texcoord
{ 	
  GLfloat t[8];
  for(int i = 0; i<8; i++) {
    t[i] = k+.5;
  }
  
  CopyRect(&divide_fp,xmin,ymin,xmax,ymax,t);
	
}

void LUDecomp::SwapRows(float a,float b) //converted
{
	swaprow_fp.SetConstant("y",b,b,b,b);

	GLfloat v[4];
	v[0] =  0; v[1] = a;
	v[2] = _m; v[3] = a;
	
	Draw(&swaprow_fp,v,v,v,8);

}

void LUDecomp::SwapCols(float a,float b) //converted
{
	swapcol_fp.SetConstant("y",b,b,b,b);

	GLfloat v[4];
	v[0] = a; v[1] =  0;

	v[2] = a; v[3] = _n;

	Draw(&swapcol_fp,v,v,v,4);
}


//----------------------------------------------------------------------------
// Function     	: LUDecomp::PingPong01
// Description	    : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::PingPong01()
 * @brief switches between two different drawing / reading buffers
 * 
 * 
 */ 
void LUDecomp::PingPong01()
{
	current01 = !current01;
	Refresh01();
	
}

//----------------------------------------------------------------------------
// Function     	: LUDecomp::Refresh01
// Description	    : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::Refresh01()
 * @brief Refreshes to the current drawing buffer (in case something else had changed the drawing buffer)
 * 
 * 
 */ 
void LUDecomp::Refresh01()  //converted
{

  glBindFramebuffer(GL_FRAMEBUFFER, current01);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureid[!current01]);
   

}



//----------------------------------------------------------------------------
// Function     	: LUDecomp
// Description	    : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::Compute()
 * @brief Computes the sparse matrix - dense vector multiplication
 * 
 */ 

void LUDecomp::Compute()  //converting   *multitexcoord
{
	
	assert( _bInitialized );

	float deltaY = 1.0f;
	float halfDeltaY = .5f;
	float rYmin  = 0.0f; 
	float rYmax  = deltaY;

	float deltaX = 1.0f;
	float halfDeltaX = .5f;
	float rXmin  = 0.0f;
	float rXmax  = deltaX;


	float index; 
	float indexi, indexj;

	int maxrow,maxcol;

	//glBindFramebuffer(GL_FRAMEBUFFER, fb);

	PingPong01();
	glClear(GL_COLOR_BUFFER_BIT);
	PingPong01();

	_CheckForGLError("Compute Init");

	for (int k = 0; k < max(_n-1,_m-1); ++k)
	{
		

		PingPong01();

		//copy column / row


		CopyRect(&copy_fp,k,k,k+1,_n, NULL);
		CopyRect(&copy_fp,k,k,_m,k+1, NULL);
		
	
		PingPong01();

		Divide(k,rXmin, rYmin + deltaY, rXmax, _m);	

		// Perform sweep of all the remaining rows
		// ie. row'(i) = row(i) - row(i)[k]*row(k)[j]

		PingPong01();

		CopyRect(&copy_fp,k+.5,k+.5,_m,k+1.5, NULL);

				
		GLfloat v[8];
		v[0] = k+1;  v[1] = k+1;
		v[2] =  _m;  v[3] = k+1;
		v[4] =  _m;  v[5] =  _n;
		v[6] = k+1;  v[7] =  _n;

		GLfloat t1[8];
		t1[0] = k+.5;  t1[1] = k+1;
		t1[2] = k+.5;  t1[3] = k+1;
		t1[4] = k+.5;  t1[5] =  _n;
		t1[6] = k+.5;  t1[7] =  _n;

		GLfloat t2[8];
		t2[0] = k+1;  t2[1] = k+.5;
		t2[2] =  _m;  t2[3] = k+.5;
		t2[4] =  _m;  t2[5] = k+.5;
		t2[6] = k+1;  t2[7] = k+.5;

		Draw(&row_fp,v,t1,t2,8);
		
		
		// Increase row coordinates
		rYmin = (k+1) * deltaY;
		rYmax = rYmin + deltaY;
		rXmin = (k+1) * deltaX;
		rXmax = rXmax + deltaX;
		
		++_currentDrawSurface;
		_CheckForGLError("LUDecomp::Compute()");


	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	_bComputed = true;
}



//----------------------------------------------------------------------------
// Function     	: LUDecomp::LoadMatrix
// Description	    : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::LoadMatrix(const std::vector<std::vector<float> >& m)
 * @brief Sets the non zero elements of the sparse matrix
 */ 
void LUDecomp::LoadMatrix(float *data)  //converted
{
 
	GLint format = (_ncomponents == 1) ? GL_LUMINANCE : GL_RGBA;
	_currentDrawSurface = 0;

	current01 = 1;


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,textureid[0]);   
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _m, _n, format, GL_FLOAT,data);	
	
	//Go into FBO mode, bind the uploaded data as the source texture
	glBindFramebuffer(GL_FRAMEBUFFER, fb[0]);
	//glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT);  assumed not needed
	glColorMask(~0,~0,~0,~0);
	//glEnable(GL_TEXTURE_RECTANGLE_NV);   assumed not needed	
	glActiveTexture( GL_TEXTURE0 );


	PingPong01();
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	++_currentDrawSurface;
	_CheckForGLError("LoadMatrix Complete");

}

//----------------------------------------------------------------------------
// Function     	: LUDecomp::GetMatrix
// Description	  : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::GetMatrix(std::vector<std::vector<float> >& m) const;
 * @brief Gets the matrix data;
 */ 
void LUDecomp::GetMatrix(std::vector<std::vector<float> >& m) const  //converted
{ 
  GLint format = (_ncomponents == 1) ? GL_LUMINANCE : GL_RGBA;
  //assert(_bComputed);

  for (int i = (int)m.size() - 1; i >= 0; --i)
    m[i].clear();
  m.clear();

  std::vector<float> data(_m * _ncomponents);

  int xoffset = 0;
  int yoffset = 0;

  glBindFramebuffer(GL_FRAMEBUFFER,(_currentDrawSurface%2) ? fb[0] : fb[1]);

	for (int i = 0; i < _n; ++i)
	{
		glReadPixels(xoffset, yoffset, _m, 1, format, GL_FLOAT, &data[0]);


		m.push_back(data);
		++yoffset;
	}
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	

}


//----------------------------------------------------------------------------
// Function     	: LUDecomp::GetMatrix
// Description	  : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::GetMatrix
 * @brief Gets the matrix 
 */ 
void LUDecomp::GetMatrix(float* m) //converted
{ 
	GLint format = (_ncomponents == 1) ? GL_LUMINANCE : GL_RGBA;
	//assert(_bComputed);


	glBindFramebuffer(GL_FRAMEBUFFER,(_currentDrawSurface%2) ? fb[0] : fb[1]);
	
	glReadPixels(0,0,_m,_n,format, GL_FLOAT,(GLvoid*)m);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
}


//----------------------------------------------------------------------------
// Function     	: static CheckForGLError
// Description	  : local check for GL error
//----------------------------------------------------------------------------
void LUDecomp::_CheckForGLError( const char *msg )   //converted
{
	GLenum errCode;
	const GLubyte *errStr;
	if ((errCode = glGetError()) != GL_NO_ERROR) 
	{
		//errStr = gluErrorString(errCode);
		fprintf(stderr,"OpenGL ERROR: 0x%x: %s\n", errCode, msg);
	}
}
