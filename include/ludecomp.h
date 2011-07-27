#ifndef __LUDECOMP_H__
#define __LUDECOMP_H__

void lugpu_sgetrf(const int *m,const int *n,float *matrix,const int *lda,int *pivot,int *info);
void lugpu_initilize(int argc, char ** argv);


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

	void OutputPixels(int x,int y,int w,int h,int k = -1);
	void Divide(int row);
	void Draw(ARBFProg* program, GLfloat* vertices, GLfloat* texcoord0, GLfloat* texcoord1, int n );

	void LoadMatrix(float *data);
	void GetMatrix(std::vector<std::vector<float> >& m) const;
	void GetMatrix(float* m); 
	bool IsInitialized() const { return _bInitialized; }
	void CopyRect(ARMFProg* program,float xmin,float ymin,float xmax,float ymax,GLfloat* t);

	void Divide(int k,float xmin,float ymin,float xmax,float ymax);

	void SetSize(int m,int n);


	void PingPong01();
	int current01;
	void Refresh01();

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

	unsigned int fb[2];
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

	


private:
};

#endif //__LUDECOMP_H__
