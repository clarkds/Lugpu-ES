/*******************notes***************/
/*  need to implement texcoord,multitexcoord and find solution for Occlusion Queries
    Also need to only allow 1 visible frame buffer at a time  */

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


void lugpu_initilize(int argc, char ** argv)   // not changing this, assuming glutES is identical
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowSize(1,1);
	glutCreateWindow("LUGPU");

	InitExts();



	lu = new LUDecomp();
	lu->Initialize(400, 400, 1,0,0);
	
}


void lugpu_sgetc2(const int *m,float *matrix,const int *lda,int *cpivot,int*rpivot,int *info) //leaving along for now,  not sure what it does
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


/**************BEGINNING CONVERSION TO OPENGLES********************/

void LUDecomp::SetSize(int m,int n)  //converted
{

	_m = m;
	_n = n;

	glActiveTexture( GL_TEXTURE0 );   
	glBindTexture(GL_TEXTURE_2D, textureid[0]);   //changed GL_TEXTURE_RECTANGE to 2D
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, _m, _n, 0, GL_LUMINANCE, FLOAT, NULL); 

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture(GL_TEXTURE_2D, textureid[1]);  
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, _m, _n, 0, GL_LUMINANCE, FLOAT, NULL);

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture(GL_TEXTURE_2D, textureid[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, _m, _n, 0, GL_LUMINANCE, FLOAT, NULL);


	glActiveTexture( GL_TEXTURE0 );
	glBindTexture(GL_TEXTURE_2D, textureid[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, _m, _n, 0, GL_LUMINANCE, FLOAT, NULL);

	glBindFramebuffer(GL_FRAMEBUFFER, fb);

	glFramebufferTexture2D(GL_FRAMEBUFFER,
                                  GL_COLOR_ATTACHMENT0,
                                  GL_TEXTURE_2D, textureid[0], 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER,
                                  GL_COLOR_ATTACHMENT1,
                                  GL_TEXTURE_2D, textureid[1], 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER,
                                  GL_COLOR_ATTACHMENT2,
                                  GL_TEXTURE_2D, textureid[2], 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER,
                                  GL_COLOR_ATTACHMENT3,
                                  GL_TEXTURE_2D, textureid[3], 0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(0, _m, 0, _n, -1, 1);
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

	glGenFramebuffers(1, &fb);	

	glGenTextures(1, &textureid[0]);
	glGenTextures(1, &textureid[1]);
	glGenTextures(1, &textureid[2]);
	glGenTextures(1, &textureid[3]);

	SetSize(_m,_n);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
void LUDecomp::CopyRect(float xmin,float ymin,float xmax,float ymax)  //converted
{
	GLuint vbuff;
	
        glGenBuffers( 8 , &vbuff);
	
	GLfloat v[8];
	v[0] = xmin;  v[1] = ymin;
	v[2] = xmin;  v[3] = ymax;
	v[4] = xmax;  v[5] = ymax;
	v[6] = xmax;  v[7] = ymin;

	GLubyte i[] = {0,1,2,   //first triangle (BL, TL ,TR)
		       0,2,3};  //second traingle(BL, TR, BR)

	glBindBuffer(GL_ARRAY_BUFFER,vbuff);
	glBufferData(GL_ARRAY_BUFFER,8*sizeof(GLfloat), v , GL_STATIC_DRAW);
      	glDrawElements(GL_TRAINGLES, 6 , GL_UNSIGNED_BYTE, i);

	glDeleteBuffers( 8 ,&vbuff);
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
        divide_fp.Bind();
	
	// need to implement glTexCoord2f here

	CopyRect(xmin,ymin,xmax,ymax);
	
	divide_fp.Release();

  /*	glBegin(GL_QUADS);
	glTexCoord2f((float)k+.5,(float)k+.5);

	glVertex2f(ymin,xmin);
	glVertex2f(ymax,xmin);
	glVertex2f(ymax,xmax);
	glVertex2f(ymin,xmax);
	glEnd();

	divide_fp.Release();      */

}

void LUDecomp::SwapRows(float a,float b) //converted
{
	swaprow_fp.Bind();
	swaprow_fp.SetConstant(0,b,b,b,b);

	GLuint vbuff;

	glGenBuffers( 4 , &vbuff);

	GLfloat v[4];
	v[0] =  0; v[1] = a;
	v[2] = _m; v[3] = a;
	
	glBindBuffer(GL_ARRAY_BUFFER, vbuff);
	glBufferData(GL_ARRAY_BUFFER, 4*sizeof(GLfloat), v , GL_STATIC_DRAW);
	glDrawArrays(GL_LINES, 0, 4);

}

void LUDecomp::SwapCols(float a,float b) //converted
{
	swapcol_fp.Bind();
	swapcol_fp.SetConstant(0,b,b,b,b);

	GLuint vbuff;

	glBenBuffers( 4, &vbuff);

	GLfloat v[4];
	v[0] = a; v[1] =  0;
	v[2] = a; v[3] = _n;

	glBindBuffer(GL_ARRAY_BUFFER, vbuff);
	glBufferData(GL_ARRAY_BUFFER, 4*sizeof(GLfloat), v , GL_STATIC_DRAW);
	glDrawArrays(GL_LINES, 0, 4);

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
	boundtexture = textureid[!current01];
	glBindTexture(GL_TEXTURE_2D,boundtexture);

	//believe these are not needed.  multiple drawing buffers has been deprecated
	//glDrawBuffer((!current01) ? GL_COLOR_ATTACHMENT0_EXT : GL_COLOR_ATTACHMENT1_EXT);
        //glReadBuffer((!current01) ? GL_COLOR_ATTACHMENT0_EXT : GL_COLOR_ATTACHMENT1_EXT);
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

void LUDecomp::Refresh23()  //converted
{
	boundtexture = textureid[!current23+2];
	glBindTexture(GL_TEXTURE_2D,boundtexture);
	
	//no longer needed.  Multiple drawing buffers deprecated
	//glDrawBuffer((!current23) ? GL_COLOR_ATTACHMENT2_EXT : GL_COLOR_ATTACHMENT3_EXT);
	//glReadBuffer((!current23) ? GL_COLOR_ATTACHMENT2_EXT : GL_COLOR_ATTACHMENT3_EXT);
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

void LUDecomp::Compute(int pivot)  //converting   *multitexcoord
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

	glBindFramebuffer(GL_FRAMEBUFFER, fb);

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


		GLuint vbuff;

		glGenBuffers( 8 , &vbuff);

		float v[];
		v[0] = k+1;  v[1] = k+1;
		v[2] =  _m;  v[3] = k+1;
		v[4] =  _m;  v[5] =  _n;
		v[6] = k+1;  v[7] =  _n;

		GLubyte i[] = {0,1,2,   //first triangle (BL, TL ,TR)
		               0,2,3};  //second traingle(BL, TR, BR)


	/*      need to implement this function call
	        glMultiTexCoord2f(GL_TEXTURE0_ARB,k+.5,k+1);
		glMultiTexCoord2f(GL_TEXTURE1_ARB,k+1,k+.5);
		glMultiTexCoord2f(GL_TEXTURE0_ARB,k+.5,k+1);
		glMultiTexCoord2f(GL_TEXTURE1_ARB,_m,k+.5);
		glMultiTexCoord2f(GL_TEXTURE0_ARB,k+.5,_n);
		glMultiTexCoord2f(GL_TEXTURE1_ARB,_m,k+.5);
		glMultiTexCoord2f(GL_TEXTURE0_ARB,k+.5,_n);
		glMultiTexCoord2f(GL_TEXTURE1_ARB,k+1,k+.5);   */

		glBindBuffer(GL_ARRAY_BUFFER,vbuff);
		glBufferData(GL_ARRAY_BUFFER,8*sizeof(GLfloat), v , GL_STATIC_DRAW);
		glDrawElements(GL_TRAINGLES, 6 , GL_UNSIGNED_BYTE, i);

		glDeleteBuffers( 8 ,&vbuff);


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
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _m, _n, format, FLOAT,data);	
	
	//Go into FBO mode, bind the uploaded data as the source texture
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	//glDrawBuffer( GL_COLOR_ATTACHMENT0_EXT);  assumed not needed
	glColorMask(~0,~0,~0,~0);
	//glEnable(GL_TEXTURE_RECTANGLE_NV);   assumed not needed	
	glActiveTexture( GL_TEXTURE0 );


	PingPong01();
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


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

	glBindFramebuffer(GL_FRAMEBUFFER,fb);
	//glReadBuffer((_currentDrawSurface%2) ? GL_COLOR_ATTACHMENT0_EXT  : GL_COLOR_ATTACHMENT1_EXT );   asssumed not needed

	for (int i = 0; i < _n; ++i)
	{
		glReadPixels(xoffset, yoffset, _m, 1, format, FLOAT, &data[0]);


		m.push_back(data);
		++yoffset;
	}
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	

}


//----------------------------------------------------------------------------
// Function     	: LUDecomp::ComputeMax
// Description	  : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::ComputeMax
 * @brief Computes the max value using a mipmap type algorithm
 */ 
void LUDecomp::ComputeMax(int k,float*maxi,float*maxj)    //converting *occlusion queries & texcoord
{
	
	static GLuint qid[4];

	static bool once=true;
	GLuint count;
	
	const int cache=0;

	int i;


	glActiveTexture(GL_TEXTURE0);

	PingPong23();
	glBindTexture(GL_TEXTURE_2D, textureid[!current01]);

	CopyRect(0,0,_m,_m);

	PingPong23();
	CopyRect(0,0,_m,_m);

	max_fp.Bind();

	if(once)
	  glGenOcclusionQueriesNV(4, qid);  //need to understand

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

			int a= nextbits;
			GLuint vbuff;

			glGenBuffers( 8 , &vbuff);

			float v1[];
			v1[0] = a+k;   v1[1] = k;
			v1[2] = i+a+k; v1[3] = k;
			v1[4] = i+a+k; v1[5] = _m+k;
			v1[6] = a+k;   v1[7] = _m+k;

		        GLubyte i1[] = {0,1,2,   //first triangle (BL, TL ,TR)
		                       0,2,3};  //second traingle(BL, TR, BR)

	       	/*	glTexCoord2f(i+a+k,k);   need to implement
			glTexCoord2f(i+i+a+k,k);
			glTexCoord2f(i+i+a+k,_m+k);
			glTexCoord2f(i+a+k,_m+k);  */

			glBindBuffer(GL_ARRAY_BUFFER,vbuff);
			glBufferData(GL_ARRAY_BUFFER,8*sizeof(GLfloat), v1 , GL_STATIC_DRAW);
			glDrawElements(GL_TRAINGLES, 6 , GL_UNSIGNED_BYTE, i1);

			glDeleteBuffers( 8 ,&vbuff);
						
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

			int a= nextbits;
		
			glGenBuffers( 8 , &vbuff);

			float v2[];
			v2[0] = k;   v2[1] = a+k;
			v2[2] = 1+k; v2[3] = a+k;
			v2[4] = 1+k; v2[5] = i+a+k;
			v2[6] = k;   v2[7] = i+a+k;

		        GLubyte i2[] = {0,1,2,   //first triangle (BL, TL ,TR)
		                       0,2,3};  //second traingle(BL, TR, BR)

	       	/*      glTexCoord2f(k,i+a+k);   need to implement
			glTexCoord2f(k,i+a+k);
			glTexCoord2f(k,i+i+a+k); 
			glTexCoord2f(k,i+i+a+k);*/

			glBindBuffer(GL_ARRAY_BUFFER,vbuff);
			glBufferData(GL_ARRAY_BUFFER,8*sizeof(GLfloat), v2 , GL_STATIC_DRAW);
			glDrawElements(GL_TRAINGLES, 6 , GL_UNSIGNED_BYTE, i2);

			glDeleteBuffers( 8 ,&vbuff);
			
		}
		
		bits = nextbits;
		bits++;
		j++;

	}

	quadtree_fp.Bind();
	quadtree_fp.SetConstant(0,k,k,k,k);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureid[!current23+2]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureid[!current01]);

	glActiveTexture(GL_TEXTURE0);



	
	PingPong23();
	glActiveTexture(GL_TEXTURE0);

	int xstart = k,ystart=k;

	bits = _m-k;



	for(int i=(bits+1) >> 1;i > 0;i = (i+1) >> 1)
	{
		glBeginOcclusionQueryNV(qid[0]);
		//quad1
		glGenBuffers( 8 , &vbuff);

			float v3[];
			v3[0] = xstart;   v3[1] = ystart;
			v3[2] = xstart+i; v3[3] = ystart;
			v3[4] = xstart+i; v3[5] = ystart+i;
			v3[6] = xstart;   v3[7] = ystart+i;

		        GLubyte i3[] = {0,1,2,   //first triangle (BL, TL ,TR)
		                       0,2,3};  //second traingle(BL, TR, BR)

	       	/*      glTexCoord2f(0,i);   need to implement
			glTexCoord2f(0,i);
			glTexCoord2f(0,i+i); 
			glTexCoord2f(0,i+i);*/

			glBindBuffer(GL_ARRAY_BUFFER,vbuff);
			glBufferData(GL_ARRAY_BUFFER,8*sizeof(GLfloat), v3 , GL_STATIC_DRAW);
			glDrawElements(GL_TRAINGLES, 6 , GL_UNSIGNED_BYTE, i3);

			glDeleteBuffers( 8 ,&vbuff);
		
		glEndOcclusionQueryNV();




		glBeginOcclusionQueryNV(qid[1]);
		//quad2
		glGenBuffers( 8 , &vbuff);
 
		float v4[];
		v4[0] = xstart+i;   v4[1] = ystart;
		v4[2] = xstart+i+i; v4[3] = ystart;
		v4[4] = xstart+i+i; v4[5] = ystart+i;
		v4[6] = xstart+i;   v4[7] = ystart+i;
		
		GLubyte i4[] = {0,1,2,   //first triangle (BL, TL ,TR)
				0,2,3};  //second traingle(BL, TR, BR)

	       	/*      glTexCoord2f(0,i);   need to implement
			glTexCoord2f(0,i);
			glTexCoord2f(0,i+i); 
			glTexCoord2f(0,i+i);*/

		glBindBuffer(GL_ARRAY_BUFFER,vbuff);
		glBufferData(GL_ARRAY_BUFFER,8*sizeof(GLfloat), v4 , GL_STATIC_DRAW);
		glDrawElements(GL_TRAINGLES, 6 , GL_UNSIGNED_BYTE, i4);

		glDeleteBuffers( 8 ,&vbuff);
		
		glEndOcclusionQueryNV();




		glBeginOcclusionQueryNV(qid[2]);
		//quad3
		glGenBuffers( 8 , &vbuff);
 
		float v5[];
		v5[0] = xstart;   v5[1] = ystart+i;
		v5[2] = xstart+i; v5[3] = ystart+i;
		v5[4] = xstart+i; v5[5] = ystart+i+i;
		v5[6] = xstart;   v5[7] = ystart+i+i;
		
		GLubyte i5[] = {0,1,2,   //first triangle (BL, TL ,TR)
				0,2,3};  //second traingle(BL, TR, BR)

	       	/*      glTexCoord2f(0,i);   need to implement
			glTexCoord2f(0,i);
			glTexCoord2f(0,i+i); 
			glTexCoord2f(0,i+i);*/

		glBindBuffer(GL_ARRAY_BUFFER,vbuff);
		glBufferData(GL_ARRAY_BUFFER,8*sizeof(GLfloat), v5 , GL_STATIC_DRAW);
		glDrawElements(GL_TRAINGLES, 6 , GL_UNSIGNED_BYTE, i5);

		glDeleteBuffers( 8 ,&vbuff);
		
		glEndOcclusionQueryNV();




		glBeginOcclusionQueryNV(qid[3]);
		//quad4
		glGenBuffers( 8 , &vbuff);
 
		float v6[];
		v6[0] = xstart+i;   v6[1] = ystart+i;
		v6[2] = xstart+i+i; v6[3] = ystart+i;
		v6[4] = xstart+i+i; v6[5] = ystart+i+i;
		v6[6] = xstart+i;   v6[7] = ystart+i+i;
		
		GLubyte i6[] = {0,1,2,   //first triangle (BL, TL ,TR)
				0,2,3};  //second traingle(BL, TR, BR)

	       	/*      glTexCoord2f(0,i);   need to implement
			glTexCoord2f(0,i);
			glTexCoord2f(0,i+i); 
			glTexCoord2f(0,i+i);*/

		glBindBuffer(GL_ARRAY_BUFFER,vbuff);
		glBufferData(GL_ARRAY_BUFFER,8*sizeof(GLfloat), v6 , GL_STATIC_DRAW);
		glDrawElements(GL_TRAINGLES, 6 , GL_UNSIGNED_BYTE, i6);

		glDeleteBuffers( 8 ,&vbuff);
		
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
void LUDecomp::ComputeMax(int k,float*maxi) //converting  *Occlusion Querries & texcoord
{
	static GLuint qid[4];

	static bool once=true;
	GLuint count;
	

	PingPong23();

	const int cache=0;

	int i;
	

	glActiveTexture(GL_TEXTURE0B);
	glBindTexture(GL_TEXTURE_2D, textureid[!current01]);
	

	CopyRect(k,k,_m,k+1);

	PingPong23();

	glBindTexture(GL_TEXTURE_2D, textureid[!current01]);

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

		GLuint vbuff;
		
		glGenBuffers( 8 , &vbuff);

		float v1[];
		v1[0] = k;   v1[1] = k;
		v1[2] = k;   v1[2] = k+1;
		v1[4] = k+i; v1[5] = k+1;
		v1[6] = k+i; v1[7] = k;

		GLubyte i1[] = {0,1,2,   //first triangle (BL, TL ,TR)
		               0,2,3};  //second traingle(BL, TR, BR)

		/*  glTexCoord2f(k+i,k);   need to implement
		    glTexCoord2f(k+i,k);
		    glTexCoord2f(k+i*2,k);
		    glTexCoord2f(k+i*2,k);     */ 

		glBindBuffer(GL_ARRAY_BUFFER,vbuff);
		glBufferData(GL_ARRAY_BUFFER,8*sizeof(GLfloat), v1 , GL_STATIC_DRAW);
		glDrawElements(GL_TRAINGLES, 6 , GL_UNSIGNED_BYTE, i1);

		glDeleteBuffers( 8 ,&vbuff);
				
	}

	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureid[!current23+2]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureid[!current01]);

	glActiveTexture(GL_TEXTURE0);

	quadtree_fp.Bind();
	quadtree_fp.SetConstant(0,k,k,k,k);

	PingPong23();

	float xstart = k+.5,ystart=k+.5;

	bits = _m-k;
	
	for(int i=(bits+1) >> 1;i > 0;i = (i+1) >> 1)
	{
		glBeginOcclusionQueryNV(qid[0]);
		//quad1
		glGenBuffers( 8 , &vbuff);

		float v2[];
		v2[0] = xstart;   v2[1] = ystart;
		v2[2] = xstart;   v2[3] = ystart+1;
		v2[4] = xstart+i; v2[5] = ystart+1;
		v2[6] = xstart+i; v2[7] = ystart;

		GLubyte i2[] = {0,1,2,   //first triangle (BL, TL ,TR)
		               0,2,3};  //second traingle(BL, TR, BR)

		/*glTexCoord2f(i,0);  need to implement
		glTexCoord2f(i,0);
		glTexCoord2f(i+i,0);
		glTexCoord2f(i+i,0); */

		glBindBuffer(GL_ARRAY_BUFFER,vbuff);
		glBufferData(GL_ARRAY_BUFFER,8*sizeof(GLfloat), v2 , GL_STATIC_DRAW);
		glDrawElements(GL_TRAINGLES, 6 , GL_UNSIGNED_BYTE, i2);

		glDeleteBuffers( 8 ,&vbuff);
		
		glEndOcclusionQueryNV();



		glBeginOcclusionQueryNV(qid[1]);
		//quad2
		glGenBuffers( 8 , &vbuff);

		float v3[];
		v3[0] = xstart+i;   v3[1] = ystart;
		v3[2] = xstart+i;   v3[3] = ystart+1;
		v3[4] = xstart+i+i; v3[5] = ystart+1;
		v3[6] = xstart+i+i; v3[7] = ystart;

		GLubyte i3[] = {0,1,2,   //first triangle (BL, TL ,TR)
		               0,2,3};  //second traingle(BL, TR, BR)

		/*glTexCoord2f(i,0);  need to implement
		glTexCoord2f(i,0);
		glTexCoord2f(i+i,0);
		glTexCoord2f(i+i,0); */

		glBindBuffer(GL_ARRAY_BUFFER,vbuff);
		glBufferData(GL_ARRAY_BUFFER,8*sizeof(GLfloat), v3 , GL_STATIC_DRAW);
		glDrawElements(GL_TRAINGLES, 6 , GL_UNSIGNED_BYTE, i3);

		glDeleteBuffers( 8 ,&vbuff);
		
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
void LUDecomp::GetMatrix(float* m) //converted
{ 
	GLint format = (_ncomponents == 1) ? GL_LUMINANCE : GL_RGBA;
	//assert(_bComputed);


	glBindFramebuffer(GL_FRAMEBUFFER,fb);
	//glReadBuffer((_currentDrawSurface%2) ? GL_COLOR_ATTACHMENT0_EXT  : GL_COLOR_ATTACHMENT1_EXT );
	glReadPixels(0,0,_m,_n,format,FLOAT,(GLvoid*)m);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
}


//----------------------------------------------------------------------------
// Function     	: LUDecomp::GetPivot
// Description	  : 
//----------------------------------------------------------------------------
/**
 * @fn LUDecomp::GetPivot
 * @brief Gets the pivot matrices
 */ 
void LUDecomp::GetPivot(int* rpivot,int *cpivot) const  //converted
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
void LUDecomp::GetPivot(int* rpivot) const  //converted
{
	for (int i = 0; i < _n; ++i)
		rpivot[i] = _rowpivot[i];
}


//----------------------------------------------------------------------------
// Function     	: static CheckForGLError
// Description	  : local check for GL error
//----------------------------------------------------------------------------
void LUDecomp::_CheckForGLError( char *msg )   //converted
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
