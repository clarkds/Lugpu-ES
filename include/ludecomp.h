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

\*****************************************************************************/#ifndef __LUDECOMP_H__
#define __LUDECOMP_H__

void lugpu_sgetrf(const int *m,const int *n,float *matrix,const int *lda,int *pivot,int *info);
void lugpu_sgetrf_fullpivot(const int *m,const int *n,float *matrix,const int *lda,int *cpivot,int*rpivot,int *info);
void lugpu_initilize(int argc, char ** argv);
void lugpu_sgetc2(const int *m,float *matrix,const int *lda,int *cpivot,int*rpivot,int *info);

#include <vector>

#include "arbfprog.h"

// forward decl
class RenderTexture;

//----------------------------------------------------------------------------
/**
 * @class LUDecomp
 * @brief A gpu implementation of gauss matrix elimination, no pivoting!
 */
class LUDecomp
{
public:
	LUDecomp();
	~LUDecomp();

	void Initialize(int m, int n, int ncomp = 1, int xblocksize = 0, int yblocksize = 0);
	void Shutdown();

	void Compute(int pivot);
	void ComputeMax(int,float*,float*);
	void ComputeMax(int,float*);

	void OutputPixels(int x,int y,int w,int h,int k = -1);
	void Divide(int row);

	void LoadMatrix(float *data);
	void GetMatrix(std::vector<std::vector<float> >& m) const;
	void GetMatrix(float* m);
	void GetPivot(int*row) const; 
	void GetPivot(int*row,int*col) const; 
	bool IsInitialized() const { return _bInitialized; }
	void CopyRect(float xmin,float ymin,float xmax,float ymax);

	void Divide(int k,float xmin,float ymin,float xmax,float ymax);

	void SetSize(int m,int n);


	void PingPong01();
	int current01;
	void Refresh01();
	void PingPong23();
	int current23;
	void Refresh23();

	int boundtexture;

protected: // methods
	void  _InitializeSlabOps();
public:
	static void _CheckForGLError( char * msg );
  
protected: // types


  void SwapRows(float a,float b);
  void SwapCols(float a,float b);

public:
	// constants
	float  _xRes;
	float  _yRes;
	int    _n;
	int    _m;
	int    _realwidth;

	unsigned int fb;
	unsigned int textureid[4];

  
	int    _ncomponents;
	int    _currentDrawSurface; // 0 is front, 1 is back

	bool            _bInitialized;
	bool            _bComputed;
  
	ARBFProg max_fp;
	ARBFProg divide_fp;
	ARBFProg copy_fp;
	ARBFProg swaprow_fp;
	ARBFProg swapcol_fp;
	ARBFProg quadtree_fp;
	ARBFProg row_fp;

	

	std::vector<int> _rowpivot,_colpivot;

private:
};

#endif //__LUDECOMP_H__
