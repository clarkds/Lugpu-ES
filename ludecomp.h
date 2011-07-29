#ifndef __LUDECOMP_H__
#define __LUDECOMP_H__

void lugpu_sgetrf(const int *m,const int *n,float *matrix,const int *lda,int *pivot,int *info);
void lugpu_initilize(int argc, char ** argv);


#include <vector>

#include "shaderprog.h"
#include "context.h"

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

	void Compute();

	void OutputPixels(int x,int y,int w,int h,int k = -1);
	void Divide(int row);
	void Draw(ShaderProg* program, GLfloat* vertices, GLfloat* texcoord0, GLfloat* texcoord1, int n );

	void LoadMatrix(float *data);
	void GetMatrix(std::vector<std::vector<float> >& m) const;
	void GetMatrix(float* m); 
	bool IsInitialized() const { return _bInitialized; }
	void CopyRect(ShaderProg* program,float xmin,float ymin,float xmax,float ymax,GLfloat* t);

	void Divide(int k,float xmin,float ymin,float xmax,float ymax);

	void SetSize(int m,int n);


	void PingPong01();
	int current01;
	void Refresh01();

	int boundtexture;

protected: // methods
	void  _InitializeSlabOps();
public:
	static void _CheckForGLError( const char * msg );
  
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
  
	ShaderProg max_fp;
	ShaderProg divide_fp;
	ShaderProg copy_fp;
	ShaderProg swaprow_fp;
	ShaderProg swapcol_fp;
	ShaderProg quadtree_fp;
	ShaderProg row_fp;

private:
    Context *context;

};

#endif //__LUDECOMP_H__
