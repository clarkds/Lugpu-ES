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

#ifndef _SHADERPROG_H
#define _SHADERPROG_H

#include <string.h>
#ifdef WIN32
#include <windows.h>

#elif defined(__APPLE__)

#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>

#else

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#endif

class ShaderProg{
public:
  ShaderProg(){}
  ~ShaderProg();

  void Load(char* vprog, char* fprog);
  void Bind();
  void BindProg();
  void Release();

  void SetConstant(char *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);

  GLuint prog_id;
  char* source;
};

#endif 
