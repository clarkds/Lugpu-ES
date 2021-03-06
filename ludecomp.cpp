/*******************notes***************/


#define for if(1) for

#include <iostream>
#include <assert.h>
#include <vector>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>



#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>

#else

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#endif

#include "ludecomp.h"

#include "common.h"
#include "context.h"
#include "stopwatch.hpp"
#include "shaderprog.h"
#include "programs.h"

using namespace std;

void printMatrix(float *mat, unsigned int M, unsigned int N) {
	for (unsigned int i = 0; i < N; i++) {
		printf("[ ");
		for (unsigned int j = 0; j < M; j++) {
			printf("%.6f ", (*mat++));
		}
		printf("]\n");
	}
}

LUDecomp * lu;

void lugpu_initilize(int argc, char ** argv)   // not changing this, assuming glutES is identical
{
	lu = new LUDecomp();
	lu->Initialize(400, 400, 1,0,0);
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
	
	printf("Loaded matrix:\n");
	printMatrix(matrix, *m, *n);
	
	lu->SetSize(*m,*n);
	lu->LoadMatrix(matrix);
	
	memset(matrix, 0, (*n) * (*m) * sizeof(float));
	
	//lu->SwapBuffers();

	//lu->Compute();
	
	lu->GetMatrix(matrix);
	printf("Got matrix:\n");
	printMatrix(matrix, *m, *n);
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
: currentBuffer(1), _xRes(1), _yRes(1), _n(0), _m(0), _ncomponents(1), _currentDrawSurface(0),_bInitialized(false), _bComputed(false)
{
    context = new Context();
    context->initialize();
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
    context->destroy();
}

/**************BEGINNING CONVERSION TO OPENGLES********************/

void LUDecomp::SetSize(int m,int n)  //converted
{

	_m = m;
	_n = n;

	glActiveTexture( GL_TEXTURE0 );  
	glBindTexture(GL_TEXTURE_2D, textureid[0]);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _m, _n, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	_CheckForGLError("Create Texture0");

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture(GL_TEXTURE_2D, textureid[1]);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _m, _n, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	_CheckForGLError("Create Texture1");
	
	glBindFramebuffer(GL_FRAMEBUFFER, fb[0]);

	glFramebufferTexture2D(GL_FRAMEBUFFER,
                                  GL_COLOR_ATTACHMENT0,
                                  GL_TEXTURE_2D, textureid[0], 0);

	_CheckForGLError("Bind Texture0");
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));
	
	glBindFramebuffer(GL_FRAMEBUFFER, fb[1]);

	glFramebufferTexture2D(GL_FRAMEBUFFER,
                                  GL_COLOR_ATTACHMENT0,
                                  GL_TEXTURE_2D, textureid[1], 0);

	_CheckForGLError("Bind Texture1");
	
	GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));

	glBindFramebuffer(GL_FRAMEBUFFER, fb[0]);

	glDepthMask(GL_FALSE);
	glClearColor(0, 0, 0, 0);
	glViewport(0, 0, _m, _n);
	_CheckForGLError("SetSize complete");

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

	if (t) {
		Draw(program, v, t, t, 8);
	} else {
		Draw(program, v, v, v, 8);
	}
	
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
	//float halfDeltaY = .5f;
	float rYmin  = 0.0f; 
	float rYmax  = deltaY;

	float deltaX = 1.0f;
	//float halfDeltaX = .5f;
	float rXmin  = 0.0f;
	float rXmax  = deltaX;


	//float index; 
	//float indexi, indexj;

	//int maxrow,maxcol;

	glBindFramebuffer(GL_FRAMEBUFFER, fb[0]);
	
	GL_CHECK();

	SwapBuffers();
	
	GL_CHECK();
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	GL_CHECK();
	
	SwapBuffers();

	_CheckForGLError("Compute Init");

	for (int k = 0; k < max(_n-1,_m-1); ++k)
	{
		
		SwapBuffers();

		//copy column / row

		CopyRect(&copy_fp,k,k,k+1,_n, NULL);
		CopyRect(&copy_fp,k,k,_m,k+1, NULL);
	
		SwapBuffers();

		Divide(k,rXmin, rYmin + deltaY, rXmax, _m);	

		// Perform sweep of all the remaining rows
		// ie. row'(i) = row(i) - row(i)[k]*row(k)[j]

		SwapBuffers();

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
// Function     	: LUDecomp::SwapBuffers
// Description	    : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::SwapBuffers()
 * @brief switches between two different drawing / reading buffers
 * 
 * 
 */ 
void LUDecomp::SwapBuffers()
{
	currentBuffer = !currentBuffer;
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, fb[currentBuffer]));
	glActiveTexture(GL_TEXTURE0);
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureid[!currentBuffer]));
	
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

	currentBuffer = 1;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureid[0]);   
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _m, _n, GL_RGBA, GL_UNSIGNED_BYTE, data);	
	
	//Go into FBO mode, bind the uploaded data as the source texture
	glBindFramebuffer(GL_FRAMEBUFFER, fb[0]);
	
	glColorMask(~0,~0,~0,~0);
	
	glActiveTexture( GL_TEXTURE0 );


	SwapBuffers();

	++_currentDrawSurface;
	_CheckForGLError("LoadMatrix Complete");

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
	
	glReadPixels(0, 0, _m, _n, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)m);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
}


//----------------------------------------------------------------------------
// Function     	: static CheckForGLError
// Description	  : local check for GL error
//----------------------------------------------------------------------------
void LUDecomp::_CheckForGLError( const char *msg )   //converted
{
	GLenum errCode;
	if ((errCode = glGetError()) != GL_NO_ERROR) 
	{
		fprintf(stderr,"OpenGL ERROR: 0x%x: %s\n", errCode, msg);
		exit(1);
	}
}
