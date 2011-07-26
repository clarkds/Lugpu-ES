/******************************************************************************\

  Copyright 2005 The University of North Carolina at Chapel Hill.
  All Rights Reserved.

  Permission to use, copy, modify and distribute this software and its
  documentation for educational, research and non-profit purposes, without
  fee, and without a written agreement is hereby granted, provided that the
  above copyright notice and the following three paragraphs appear in all
  copies. Any use in a commercial organization requires a separate license.

  IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL BE LIABLE
  TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
  DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND
  ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF NORTH CAROLINA HAVE BEEN
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.


  Permission to use, copy, modify and distribute this software and its
  documentation for educational, research and non-profit purposes, without
  fee, and without a written agreement is hereby granted, provided that the
  above copyright notice and the following three paragraphs appear in all
  copies.

  THE UNIVERSITY OF NORTH CAROLINA SPECIFICALLY DISCLAIM ANY WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
  FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN
  "AS IS" BASIS, AND THE UNIVERSITY OF NORTH CAROLINA HAS NO OBLIGATION TO
  PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


   ---------------------------------
  |Please send all BUG REPORTS to:  |
  |                                 |
  |   geom@cs.unc.edu               |
  |                                 |
   ---------------------------------


  The authors may be contacted via:

  US Mail:         N. Govindaraju, N. Raghuvanshi or D. Manocha
                       Department of Computer Science
                       Sitterson Hall, CB #3175
                       University of North Carolina
                       Chapel Hill, NC 27599-3175

\*****************************************************************************/
#define for if(1) for

#include <iostream>
#include <assert.h>
#include <vector>
#include <cmath>

#include <GL/glut.h>
#include "defines.h"

#include "common.h"
#include "ludecomp.h"
#include "stopwatch.hpp"

#include "arbfprog.h"

#include "programs.h"

using namespace std;

LUDecomp * lu;


void lugpu_initilize(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(1,1);
	glutCreateWindow("LUGPU");

	InitExts();



	lu = new LUDecomp();
	lu->Initialize(400, 400, 1,0,0);
	
}


void lugpu_sgetc2(const int *m,float *matrix,const int *lda,int *cpivot,int*rpivot,int *info)
{

	*info = 0;
	if(*m < 0)
		*info = -1;
	if(*lda < max(1,*m))
		*info = -4;

	if(*info != 0)
	{
		/****TODO*****/
		//call XERBLA
		return;
	}

	lu->SetSize(*m,*m);
	lu->LoadMatrix(matrix);

	lu->Compute(LUGPU_FULLPIVOT);
	lu->GetPivot(cpivot,rpivot);

	for(int i=0;i < lu->_n;i++)
	{
		cpivot[i]++;
		rpivot[i]++;
	}

	lu->GetMatrix(matrix);
}


void lugpu_sgetrf(const int *m,const int *n,float *matrix,const int *lda,int *pivot,int *info)
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

	lu->Compute(LUGPU_PARTIALPIVOT);

	
	lu->GetPivot(pivot);

	for(int i=0;i < lu->_n;i++)
	{
		if(pivot[i] < i)
			pivot[i] = i;

		pivot[i]++;
	}

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




void LUDecomp::SetSize(int m,int n)
{

	_m = m;
	_n = n;

	glActiveTextureARB( GL_TEXTURE0_ARB );
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, textureid[0]);
	glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_R32_NV, _m, _n, 0, GL_RED, GL_FLOAT, NULL);

	glActiveTextureARB( GL_TEXTURE0_ARB );
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, textureid[1]);
	glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_R32_NV, _m, _n, 0, GL_RED, GL_FLOAT, NULL);

	glActiveTextureARB( GL_TEXTURE0_ARB );
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, textureid[2]);
	glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_R32_NV, _m, _n, 0, GL_RED, GL_FLOAT, NULL);


	glActiveTextureARB( GL_TEXTURE0_ARB );
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, textureid[3]);
	glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_FLOAT_R32_NV, _m, _n, 0, GL_RED, GL_FLOAT, NULL);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_RECTANGLE_NV, textureid[0], 0);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT1_EXT,
                                  GL_TEXTURE_RECTANGLE_NV, textureid[1], 0);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT2_EXT,
                                  GL_TEXTURE_RECTANGLE_NV, textureid[2], 0);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_COLOR_ATTACHMENT3_EXT,
                                  GL_TEXTURE_RECTANGLE_NV, textureid[3], 0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, _m, 0, _n);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glClearColor(0, 0, 0, 0);
	glViewport(0, 0, _m, _n);

	int i;
	_rowpivot.clear();
	_rowpivot.resize(_n);
	for (i = 0; i < _n; ++i)
		_rowpivot[i] = i;

	_colpivot.clear();
	_colpivot.resize(_m);
	for (i = 0; i < _m; ++i)
		_colpivot[i] = i;

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
void LUDecomp::Initialize(int m, int n, int ncomp, int xblocksize, int yblocksize)
{
	lu->_InitializeSlabOps();
	
	_m = m / ncomp;
	_n = n;
	_ncomponents = ncomp;
	assert(_ncomponents == 1 || _ncomponents == 4);

	_xRes = (xblocksize) ? (float)xblocksize / _n : 1.0;
	_yRes = (yblocksize) ? (float)yblocksize / _m : 1.0;

	_bInitialized = true;

	glGenFramebuffersEXT(1, &fb);	

	glGenTextures(1, &textureid[0]);
	glGenTextures(1, &textureid[1]);
	glGenTextures(1, &textureid[2]);
	glGenTextures(1, &textureid[3]);

	SetSize(_m,_n);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
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

	max_fp.Load(max_op);
	swapcol_fp.Load(swapcol_op);
	swaprow_fp.Load(swaprow_op);
	divide_fp.Load(divide_op);
	copy_fp.Load(copy_op);
	quadtree_fp.Load(quadtree_op);
	row_fp.Load(row_op);
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
void LUDecomp::CopyRect(float xmin,float ymin,float xmax,float ymax)
{
	copy_fp.Bind();

	glBegin(GL_QUADS);

	glVertex2f(xmin,ymin);
	glVertex2f(xmax,ymin);
	glVertex2f(xmax,ymax);
	glVertex2f(xmin,ymax);
	glEnd();
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
void LUDecomp::Divide(int k,float xmin,float ymin,float xmax,float ymax)
{
	divide_fp.Bind();

	glBegin(GL_QUADS);
	glTexCoord2f((float)k+.5,(float)k+.5);

	glVertex2f(ymin,xmin);
	glVertex2f(ymax,xmin);
	glVertex2f(ymax,xmax);
	glVertex2f(ymin,xmax);
	glEnd();

	divide_fp.Release();
}

void LUDecomp::SwapRows(float a,float b)
{
	swaprow_fp.Bind();
	swaprow_fp.SetConstant(0,b,b,b,b);
	glBegin(GL_LINES);
	glVertex2f(0,a);
	glVertex2f(_m,a);
	glEnd();

}

void LUDecomp::SwapCols(float a,float b)
{
	swapcol_fp.Bind();
	swapcol_fp.SetConstant(0,b,b,b,b);
	glBegin(GL_LINES);
	glVertex2f(a,0);
	glVertex2f(a,_n);
	glEnd();
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
void LUDecomp::Refresh01()
{
	glDrawBuffer((!current01) ? GL_COLOR_ATTACHMENT0_EXT : GL_COLOR_ATTACHMENT1_EXT);
	glReadBuffer((!current01) ? GL_COLOR_ATTACHMENT0_EXT : GL_COLOR_ATTACHMENT1_EXT);

	boundtexture = textureid[!current01];
	glBindTexture(GL_TEXTURE_RECTANGLE_NV,boundtexture);
}

//----------------------------------------------------------------------------
// Function     	: LUDecomp::PingPong23
// Description	    : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::PingPong23()
 * @brief switches between two different drawing / reading buffers
 * 
 * 
 */ 

void LUDecomp::PingPong23()
{
	current23 = !current23;
	Refresh23();
}
//----------------------------------------------------------------------------
// Function     	: LUDecomp::Refresh23
// Description	    : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::Refresh23()
 * @brief Refreshes to the current drawing buffer (in case something else had changed the drawing buffer)
 * 
 * 
 */ 

void LUDecomp::Refresh23()
{
	glDrawBuffer((!current23) ? GL_COLOR_ATTACHMENT2_EXT : GL_COLOR_ATTACHMENT3_EXT);
	glReadBuffer((!current23) ? GL_COLOR_ATTACHMENT2_EXT : GL_COLOR_ATTACHMENT3_EXT);

	boundtexture = textureid[!current23+2];
	glBindTexture(GL_TEXTURE_RECTANGLE_NV,boundtexture);
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

void LUDecomp::Compute(int pivot)
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

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);

	PingPong01();
	glClear(GL_COLOR_BUFFER_BIT);
	PingPong01();

	for (int k = 0; k < max(_n-1,_m-1); ++k)
	{
		

		PingPong01();

		//copy column / row


		CopyRect(k,k,k+1,_n);
		CopyRect(k,k,_m,k+1);
		

		if(pivot != LUGPU_NOPIVOT)
		{
			index = k;
			// Max computation
			// Store max indices in row k+1
			static bool once = true;
			
			if(pivot == LUGPU_FULLPIVOT)
			{
				ComputeMax(k,&indexj,&indexi);
			}
			else
			{
				Refresh01();
				ComputeMax(k,&indexi);
			}		

			Refresh01();

			maxrow = floor(indexi);
			maxcol = floor(indexj);

		  // Swap rows
			if (maxrow != k) 
			{

#if MKL_PIVOT
				_rowpivot[k] = _rowpivot[maxrow];
#else
				std::swap(_rowpivot[k], _rowpivot[maxrow]);
#endif

				// kth col, read from maxcol
				SwapCols(rXmin + halfDeltaX,float(maxrow+.5f));

				// maxcol-th col, read from kth col
				SwapCols((deltaX * maxrow) + halfDeltaX,float(k + 0.5f));

				// Copy back
				PingPong01();
				
				// Copy col k

				CopyRect(rXmin,0,rXmax,_n);

				// Copy col maxcol
				CopyRect(deltaX*maxrow, 0.0,deltaX*maxrow+deltaX,_n);

				// Restore back/front buffers

				PingPong01();
			}


			if(pivot == LUGPU_FULLPIVOT && maxcol != k) 
			{

#if MKL_PIVOT
				_colpivot[k] = _colpivot[maxcol];
#else
				std::swap(_colpivot[k], _colpivot[maxcol]);
#endif
				// kth row, read from maxrow

				SwapRows(rYmin+halfDeltaY,float(maxcol+.5f));

				// maxrow-th row, read from kth row

				SwapRows((deltaY * maxcol) + halfDeltaY,float(k+.5f));

				// Copy back
				PingPong01();

				// Copy row k
				CopyRect(0.0, rYmin, _m, rYmax);

				// Copy row maxrow
				CopyRect(0.0, (deltaY * maxcol), _m, (deltaY * maxcol) + deltaY);

				PingPong01();
			}
		}

		PingPong01();

		Divide(k,rXmin, rYmin + deltaY, rXmax, _m);	

		// Perform sweep of all the remaining rows
		// ie. row'(i) = row(i) - row(i)[k]*row(k)[j]

		PingPong01();

		CopyRect(k+.5,k+.5,_m,k+1.5);

		row_fp.Bind();

		glBegin(GL_QUADS);
		glMultiTexCoord2f(GL_TEXTURE0_ARB,k+.5,k+1);
		glMultiTexCoord2f(GL_TEXTURE1_ARB,k+1,k+.5);
		glVertex2f(k+1,k+1);

		glMultiTexCoord2f(GL_TEXTURE0_ARB,k+.5,k+1);
		glMultiTexCoord2f(GL_TEXTURE1_ARB,_m,k+.5);
		glVertex2f(_m,k+1);

		glMultiTexCoord2f(GL_TEXTURE0_ARB,k+.5,_n);
		glMultiTexCoord2f(GL_TEXTURE1_ARB,_m,k+.5);
		glVertex2f(_m,_n);

		glMultiTexCoord2f(GL_TEXTURE0_ARB,k+.5,_n);
		glMultiTexCoord2f(GL_TEXTURE1_ARB,k+1,k+.5);
		glVertex2f(k+1,_n);

		glEnd();

		// Increase row coordinates
		rYmin = (k+1) * deltaY;
		rYmax = rYmin + deltaY;
		rXmin = (k+1) * deltaX;
		rXmax = rXmax + deltaX;
		
		++_currentDrawSurface;
		_CheckForGLError("LUDecomp::Compute()");


	}
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

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
void LUDecomp::LoadMatrix(float *data)
{
 
	GLint format = (_ncomponents == 1) ? GL_RED : GL_RGBA;
	_currentDrawSurface = 0;

	current01 = 1;


	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV,textureid[0]);   
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, _m, _n, format, GL_FLOAT,data);	
	
	//Go into FBO mode, bind the uploaded data as the source texture
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
	glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT);  
	glColorMask(~0,~0,~0,~0);
	glEnable(GL_TEXTURE_RECTANGLE_NV);	
	glActiveTextureARB( GL_TEXTURE0_ARB );


	PingPong01();
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);


	++_currentDrawSurface;

}

//----------------------------------------------------------------------------
// Function     	: LUDecomp::GetMatrix
// Description	  : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::GetMatrix(std::vector<std::vector<float> >& m) const;
 * @brief Gets the matrix data;
 */ 
void LUDecomp::GetMatrix(std::vector<std::vector<float> >& m) const
{ 
  GLint format = (_ncomponents == 1) ? GL_RED : GL_RGBA;
  //assert(_bComputed);

  for (int i = (int)m.size() - 1; i >= 0; --i)
    m[i].clear();
  m.clear();

  std::vector<float> data(_m * _ncomponents);

  int xoffset = 0;
  int yoffset = 0;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fb);
	glReadBuffer((_currentDrawSurface%2) ? GL_COLOR_ATTACHMENT0_EXT  : GL_COLOR_ATTACHMENT1_EXT );
	for (int i = 0; i < _n; ++i)
	{
		glReadPixels(xoffset, yoffset, _m, 1, format, GL_FLOAT, &data[0]);


		m.push_back(data);
		++yoffset;
	}
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
	

}


//----------------------------------------------------------------------------
// Function     	: LUDecomp::ComputeMax
// Description	  : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::ComputeMax
 * @brief Computes the max value using a mipmap type algorithm
 */ 
void LUDecomp::ComputeMax(int k,float*maxi,float*maxj) 
{
	
	static GLuint qid[4];

	static bool once=true;
	GLuint count;
	
	const int cache=0;

	int i;


	glActiveTextureARB(GL_TEXTURE0_ARB);

	PingPong23();
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, textureid[!current01]);

	CopyRect(0,0,_m,_m);

	PingPong23();
	CopyRect(0,0,_m,_m);

	max_fp.Bind();

	if(once)
		glGenOcclusionQueriesNV(4, qid);

	int bits = _m-k;

	//put the max for each row in the column
	for(int j=31;j > 0;j--)
	{
		if(!(bits & (1 << j)))
			continue;

		int nextbits = bits & (~(1 << j));
		int cur = (1 << (j-1));

		int end=1;
		if(j == 1)
			end = 2;

		
		for(int l=0;l < end;l++)
		for (i=cur;i > 0;i = i >> 1)
		{

			PingPong23();
			

			glBegin(GL_QUADS);
			{
				int a= nextbits;
				glTexCoord2f(i+a+k,k);
				glVertex2f(a+k,k);

				glTexCoord2f(i+i+a+k,k);
				glVertex2f(i+a+k,k);

				glTexCoord2f(i+i+a+k,_m+k);
				glVertex2f(i+a+k,_m+k);

				glTexCoord2f(i+a+k,_m+k);
				glVertex2f(a+k,_m+k);
			}
			glEnd();
			
		}
		
		bits = nextbits;
		bits++;
		j++;
	}

	bits = _m-k;
	

	//find the max over the whole column
	for(int j=31;j > 0;j--)
	{
		if(!(bits & (1 << j)))
			continue;

		int nextbits = bits & (~(1 << j));
		int cur = (1 << (j-1));
		for (i=cur;i > 0;i = i >> 1)
		{
		
			PingPong23();

			glBegin(GL_QUADS);
			{
				
				int a= nextbits;
				glTexCoord2f(k,i+a+k);
				glVertex2f(k,a+k);
				glTexCoord2f(k,i+a+k);
				glVertex2f(1+k,a+k);
				glTexCoord2f(k,i+i+a+k);
				glVertex2f(1+k,i+a+k);
				glTexCoord2f(k,i+i+a+k);
				glVertex2f(k,i+a+k);
			}
			glEnd();
		}
		
		bits = nextbits;
		bits++;
		j++;

	}

	quadtree_fp.Bind();
	quadtree_fp.SetConstant(0,k,k,k,k);


	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, textureid[!current23+2]);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, textureid[!current01]);

	glActiveTextureARB(GL_TEXTURE0_ARB);



	
	PingPong23();
	glActiveTextureARB(GL_TEXTURE0_ARB);

	int xstart = k,ystart=k;

	bits = _m-k;

	for(int i=(bits+1) >> 1;i > 0;i = (i+1) >> 1)
	{
		glBeginOcclusionQueryNV(qid[0]);
		glBegin(GL_QUADS);

			//quad1
			glTexCoord2f(0,i);
			glVertex2f(xstart,ystart);

			glTexCoord2f(0,i);
			glVertex2f(xstart+i,ystart);

			glTexCoord2f(0,i+i);
			glVertex2f(xstart+i,ystart+i);

			glTexCoord2f(0,i+i);
			glVertex2f(xstart,ystart+i);

		glEnd();
		glEndOcclusionQueryNV();


		glBeginOcclusionQueryNV(qid[1]);
		glBegin(GL_QUADS);
			//quad2
			glTexCoord2f(0,i);
			glVertex2f(xstart+i,ystart);

			glTexCoord2f(0,i);
			glVertex2f(xstart+i*2,ystart);

			glTexCoord2f(0,i+i);
			glVertex2f(xstart+i*2,ystart+i);

			glTexCoord2f(0,i+i);
			glVertex2f(xstart+i,ystart+i);
		glEnd();
		glEndOcclusionQueryNV();


		glBeginOcclusionQueryNV(qid[2]);
		glBegin(GL_QUADS);
			//quad3
			glTexCoord2f(0,i);
			glVertex2f(xstart,ystart+i);

			glTexCoord2f(0,i);
			glVertex2f(xstart+i,ystart+i);

			glTexCoord2f(0,i+i);
			glVertex2f(xstart+i,ystart+i+i);

			glTexCoord2f(0,i+i);
			glVertex2f(xstart,ystart+i+i);
		glEnd();
		glEndOcclusionQueryNV();

		glBeginOcclusionQueryNV(qid[3]);
		glBegin(GL_QUADS);
			//quad4
			glTexCoord2f(0,i);
			glVertex2f(xstart+i,ystart+i);

			glTexCoord2f(0,i);
			glVertex2f(xstart+i+i,ystart+i);

			glTexCoord2f(0,i+i);
			glVertex2f(xstart+i+i,ystart+i+i);

			glTexCoord2f(0,i+i);
			glVertex2f(xstart+i,ystart+i+i);
		glEnd();
		glEndOcclusionQueryNV();

		for(int j=0;j < 4;j++)
		{
			glGetOcclusionQueryuivNV(qid[j], GL_PIXEL_COUNT_NV, &count);
			
			if(count >= 1)
			{
				xstart+= (j%2)*i;
				ystart+= (j/2)*i;
				break;
			}
		}

		if(i == 1)
			break;
	}

	*maxj = xstart;
	*maxi = ystart;

	glViewport(0, 0, _m, _n);
	once=false;
}



//----------------------------------------------------------------------------
// Function     	: LUDecomp::ComputeMax
// Description	  : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::ComputeMax
 * @brief Computes the max value using a mipmap type algorithm
 */ 
void LUDecomp::ComputeMax(int k,float*maxi) 
{
	static GLuint qid[4];

	static bool once=true;
	GLuint count;
	

	PingPong23();

	const int cache=0;

	int i;
	

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, textureid[!current01]);
	

	CopyRect(k,k,_m,k+1);

	PingPong23();

	glBindTexture(GL_TEXTURE_RECTANGLE_NV, textureid[!current01]);

	CopyRect(k,k,_m,k+1);

	max_fp.Bind();

	if(once)
		glGenOcclusionQueriesNV(4, qid);

	int bits = _m-k;

	for(int j=31;j > 0;j--)
	{
		if(!(bits & (1 << j)))
			continue;

		if(bits & (bits-1))
			bits = 1 << (j+1);
		
		break;
	}
	for (i=bits;i > 0;i = i >> 1)
	{

		PingPong23();
		
		_currentDrawSurface++;

		glBegin(GL_QUADS);
		{
			glTexCoord2f(k+i,k);
			glVertex2f(k,k);

			glTexCoord2f(k+i,k);
			glVertex2f(k,k+1);
			
			glTexCoord2f(k+i*2,k);
			glVertex2f(k+i,k+1);
			
			glTexCoord2f(k+i*2,k);
			glVertex2f(k+i,k);
		}
		glEnd();
	}

	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, textureid[!current23+2]);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, textureid[!current01]);

	glActiveTextureARB(GL_TEXTURE0_ARB);

	quadtree_fp.Bind();
	quadtree_fp.SetConstant(0,k,k,k,k);

	PingPong23();

	float xstart = k+.5,ystart=k+.5;

	bits = _m-k;
	
	for(int i=(bits+1) >> 1;i > 0;i = (i+1) >> 1)
	{
		glBeginOcclusionQueryNV(qid[0]);
		glBegin(GL_QUADS);

			//quad1

			glTexCoord2f(i,0);
			glVertex2f(xstart,ystart);

			glTexCoord2f(i,0);
			glVertex2f(xstart,ystart+1);

			glTexCoord2f(i+i,0);
			glVertex2f(xstart+i,ystart+1);

			glTexCoord2f(i+i,0);
			glVertex2f(xstart+i,ystart);

		glEnd();
		glEndOcclusionQueryNV();

		glBeginOcclusionQueryNV(qid[1]);
		glBegin(GL_QUADS);
			//quad2
			glTexCoord2f(i,0);
			glVertex2f(xstart+i,ystart);

			glTexCoord2f(i,0);
			glVertex2f(xstart+i,ystart+1);

			glTexCoord2f(i+i,0);
			glVertex2f(xstart+i+i,ystart+1);

			glTexCoord2f(i+i,0);
			glVertex2f(xstart+i+i,ystart);
		glEnd();
		glEndOcclusionQueryNV();

		for(int j=0;j < 2;j++)
		{
			glGetOcclusionQueryuivNV(qid[j], GL_PIXEL_COUNT_NV, &count);
			
			if(count >= 1)
			{
				xstart+= j*i;
				break;
			}
		}

		if(i == 1)
			break;
	}
	
	*maxi = xstart-.5;

	glViewport(0, 0, _m, _n);
	
	once=false;


}


//----------------------------------------------------------------------------
// Function     	: LUDecomp::GetMatrix
// Description	  : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::GetMatrix
 * @brief Gets the matrix 
 */ 
void LUDecomp::GetMatrix(float* m) 
{ 
	GLint format = (_ncomponents == 1) ? GL_RED : GL_RGBA;
	//assert(_bComputed);


	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fb);
	glReadBuffer((_currentDrawSurface%2) ? GL_COLOR_ATTACHMENT0_EXT  : GL_COLOR_ATTACHMENT1_EXT );
	glReadPixels(0,0,_m,_n,format,GL_FLOAT,(GLvoid*)m);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
}


//----------------------------------------------------------------------------
// Function     	: LUDecomp::GetPivot
// Description	  : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::GetPivot
 * @brief Gets the pivot matrices
 */ 
void LUDecomp::GetPivot(int* rpivot,int *cpivot) const
{
	for (int i = 0; i < _n; ++i)
		rpivot[i] = _rowpivot[i];
	for (int i = 0; i < _m; ++i)
		cpivot[i] = _colpivot[i];
}

//----------------------------------------------------------------------------
// Function     	: LUDecomp::GetPivot
// Description	  : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::GetPivot
 * @brief Gets the pivot matrix 
 */ 
void LUDecomp::GetPivot(int* rpivot) const
{
	for (int i = 0; i < _n; ++i)
		rpivot[i] = _rowpivot[i];
}


//----------------------------------------------------------------------------
// Function     	: static CheckForGLError
// Description	  : local check for GL error
//----------------------------------------------------------------------------
void LUDecomp::_CheckForGLError( char *msg )
{
#if defined(DEBUG) | defined(_DEBUG)
	GLenum errCode;
	const GLubyte *errStr;
	if ((errCode = glGetError()) != GL_NO_ERROR) 
	{
		errStr = gluErrorString(errCode);
		fprintf(stderr,"OpenGL ERROR: %s: %s\n", errStr, msg);
	}
#endif
}
