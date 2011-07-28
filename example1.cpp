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

\*****************************************************************************/#define for if(true) for


#include <vector>
using std::vector;
#include <algorithm>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <time.h>

#include <math.h>

#include <gl/glut.h>
#include "stopwatch.hpp"


#include "ludecomp.h"

using namespace std;

void lehmer (float *mat, unsigned int M,unsigned int N, int ncomp = 1, bool append = false)
{
  float *pmat = mat;
  for (unsigned int i = 0; i < N; i++) {
    for (unsigned int j = 0; j < M; j++) {
      for (int c = 0; c < ncomp; ++c)
	  {
        (*pmat++) = ((j>=i) ? ((float)i+1)/((float)j+1) : ((float)j+1)/((float)i+1));
	  }

    }
    if (append)
        for (int c = 0; c < ncomp; ++c)
            (*pmat++) = -3.0f;
  }
}
//--------------------------------------------------------------------------------------------
int main (int argc, char *argv[])
{
	cout.precision(4);

	int n, m;
	float *gpumat;

	lugpu_initilize(argc, argv);

	n = 3500;
	{

		gpumat = new float[n * n];
		
		lehmer(gpumat,n,n,1,false);

		int info;

		int *cpivot = new int[n];
		int *rpivot = new int[n];

		Stopwatch gputimer("gpu timer");
		gputimer.Start();

		lugpu_sgetc2(&n,gpumat,&n,rpivot,cpivot,&info);

		gputimer.Stop();

		cout << gputimer  << endl;
	
	}
	return 1;
}