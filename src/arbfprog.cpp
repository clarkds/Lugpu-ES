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
#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#else
#include <GL/glx.h>
#endif

#include <GL/glu.h>
#include "arbfprog.h"
#include "defines.h"


typedef void (APIENTRY * PFNGenProgramsARB) (GLsizei n, unsigned int *ids);
typedef void (APIENTRY * PFNLoadProgramARB) (int target, unsigned int id, GLsizei len, const unsigned char *program);
typedef void (APIENTRY * PFNGLProgramStringARB) (GLenum target, GLenum format, GLsizei len, const void* string);
typedef void (APIENTRY * PFNGLBindProgramARB) (GLint target, GLuint id);
typedef void (APIENTRY  *PFNGLDeleteProgramsARB) (GLsizei n, const GLuint *);

#define GL_FRAGMENT_PROGRAM_ARB 0x8804
#define GL_MAX_TEXTURE_COORDS_ARB 0x8871
#define GL_MAX_TEXTURE_IMAGE_UNITS_ARB 0x8872
#define GL_FRAGMENT_PROGRAM_BINDING_ARB 0x8873
#define GL_PROGRAM_ERROR_STRING_ARB 0x8874

PFNGenProgramsARB glGenProgramsARB;
PFNLoadProgramARB glLoadProgramARB;
PFNGLBindProgramARB glBindProgramARB;
PFNGLProgramStringARB glProgramStringARB;
PFNGLDeleteProgramsARB glDeleteProgramsARB;


PFNGLGENOCCLUSIONQUERIESNVPROC glGenOcclusionQueriesNV ;
PFNGLDELETEOCCLUSIONQUERIESNVPROC glDeleteOcclusionQueriesNV ;
PFNGLISOCCLUSIONQUERYNVPROC glIsOcclusionQueryNV ;
PFNGLBEGINOCCLUSIONQUERYNVPROC glBeginOcclusionQueryNV ;
PFNGLENDOCCLUSIONQUERYNVPROC glEndOcclusionQueryNV ;
PFNGLGETOCCLUSIONQUERYIVNVPROC glGetOcclusionQueryivNV ;
PFNGLGETOCCLUSIONQUERYUIVNVPROC glGetOcclusionQueryuivNV;

PFNGLMULTITEXCOORD2FPROC	glMultiTexCoord2f;

PFNGLPROGRAMLOCALPARAMETER4FARBPROC glProgramLocalParameter4fARB;

PFNGLActiveTextureARB glActiveTextureARB;
PFNIsRenderbufferEXT glIsRenderbufferEXT;
PFNBindRenderbufferEXT glBindRenderbufferEXT;
PFNDeleteRenderbuffersEXT glDeleteRenderbuffersEXT;
PFNGenRenderbuffersEXT glGenRenderbuffersEXT;
PFNRenderbufferStorageEXT glRenderbufferStorageEXT;
PFNGetRenderbufferParameterivEXT glGetRenderbufferParameterivEXT;
PFNIsFramebufferEXT glIsFramebufferEXT;
PFNDeleteFramebuffersEXT glDeleteFramebuffersEXT;

PFNBindFramebufferEXT glBindFramebufferEXT;
PFNGenFramebuffersEXT glGenFramebuffersEXT;
PFNFramebufferTexture2DEXT glFramebufferTexture2DEXT;

#define GL_PROGRAM_FORMAT_ASCII_ARB 0x8875

void InitExts(){
#ifdef WIN32
glIsRenderbufferEXT = (PFNIsRenderbufferEXT) wglGetProcAddress("glIsRenderbufferEXT");
glBindRenderbufferEXT = (PFNBindRenderbufferEXT)  wglGetProcAddress("glBindRenderbufferEXT");
glDeleteRenderbuffersEXT = (PFNDeleteRenderbuffersEXT)  wglGetProcAddress("glDeleteRenderbuffersEXT");
glGenRenderbuffersEXT = (PFNGenRenderbuffersEXT ) wglGetProcAddress("glGenRenderbuffersEXT");
glRenderbufferStorageEXT = (PFNRenderbufferStorageEXT ) wglGetProcAddress("glRenderbufferStorageEXT");
glGetRenderbufferParameterivEXT =(PFNGetRenderbufferParameterivEXT ) wglGetProcAddress("glGetRenderbufferParameterivEXT");
glIsFramebufferEXT =(PFNIsFramebufferEXT ) wglGetProcAddress("glIsFramebufferEXT");
glBindFramebufferEXT =(PFNBindFramebufferEXT ) wglGetProcAddress("glBindFramebufferEXT");
glDeleteFramebuffersEXT =(PFNDeleteFramebuffersEXT ) wglGetProcAddress("glDeleteFramebuffersEXT");
glGenFramebuffersEXT =(PFNGenFramebuffersEXT ) wglGetProcAddress("glGenFramebuffersEXT");
glFramebufferTexture2DEXT = (PFNFramebufferTexture2DEXT) wglGetProcAddress("glFramebufferTexture2DEXT");

glActiveTextureARB = (PFNGLActiveTextureARB) wglGetProcAddress("glActiveTextureARB");
glGenProgramsARB = (PFNGenProgramsARB) wglGetProcAddress("glGenProgramsARB");
glLoadProgramARB= (PFNLoadProgramARB) wglGetProcAddress("glLoadProgramARB");
glBindProgramARB = (PFNGLBindProgramARB) wglGetProcAddress("glBindProgramARB");
glProgramStringARB = (PFNGLProgramStringARB) wglGetProcAddress("glProgramStringARB");
glDeleteProgramsARB = (PFNGLDeleteProgramsARB) wglGetProcAddress("glDeleteProgramsARB");

glMultiTexCoord2f = (PFNGLMULTITEXCOORD2FPROC) wglGetProcAddress("glMultiTexCoord2f");
glGenOcclusionQueriesNV = (PFNGLGENOCCLUSIONQUERIESNVPROC) wglGetProcAddress("glGenOcclusionQueriesNV");
glDeleteOcclusionQueriesNV = (PFNGLDELETEOCCLUSIONQUERIESNVPROC) wglGetProcAddress("glDeleteOcclusionQueriesNV");
glIsOcclusionQueryNV = (PFNGLISOCCLUSIONQUERYNVPROC) wglGetProcAddress("glIsOcclusionQueryNV");
glBeginOcclusionQueryNV = (PFNGLBEGINOCCLUSIONQUERYNVPROC) wglGetProcAddress("glBeginOcclusionQueryNV");
glEndOcclusionQueryNV = (PFNGLENDOCCLUSIONQUERYNVPROC) wglGetProcAddress("glEndOcclusionQueryNV");
glGetOcclusionQueryivNV = (PFNGLGETOCCLUSIONQUERYIVNVPROC) wglGetProcAddress("glGetOcclusionQueryivNV");
glGetOcclusionQueryuivNV = (PFNGLGETOCCLUSIONQUERYUIVNVPROC) wglGetProcAddress("glGetOcclusionQueryuivNV");

glBindFramebufferEXT = (PFNBindFramebufferEXT) wglGetProcAddress("glBindFramebufferEXT");
glGenFramebuffersEXT = (PFNGenFramebuffersEXT) wglGetProcAddress("glGenFramebuffersEXT");
glFramebufferTexture2DEXT = (PFNFramebufferTexture2DEXT) wglGetProcAddress("glFramebufferTexture2DEXT");

glProgramLocalParameter4fARB = (PFNGLPROGRAMLOCALPARAMETER4FARBPROC) wglGetProcAddress("glProgramLocalParameter4fARB");

#else

glIsRenderbufferEXT = (PFNIsRenderbufferEXT) glXGetProcAddressARB((const GLubyte *) "glIsRenderbufferEXT");
glBindRenderbufferEXT = (PFNBindRenderbufferEXT)  glXGetProcAddressARB((const GLubyte *) "glBindRenderbufferEXT");
glDeleteRenderbuffersEXT = (PFNDeleteRenderbuffersEXT)  glXGetProcAddressARB((const GLubyte *) "glDeleteRenderbuffersEXT");
glGenRenderbuffersEXT = (PFNGenRenderbuffersEXT ) glXGetProcAddressARB((const GLubyte *) "glGenRenderbuffersEXT");
glRenderbufferStorageEXT = (PFNRenderbufferStorageEXT ) glXGetProcAddressARB((const GLubyte *) "glRenderbufferStorageEXT");
glGetRenderbufferParameterivEXT =(PFNGetRenderbufferParameterivEXT ) glXGetProcAddressARB((const GLubyte *) "glGetRenderbufferParameterivEXT");
glIsFramebufferEXT =(PFNIsFramebufferEXT ) glXGetProcAddressARB((const GLubyte *) "glIsFramebufferEXT");
glBindFramebufferEXT =(PFNBindFramebufferEXT ) glXGetProcAddressARB((const GLubyte *) "glBindFramebufferEXT");
glDeleteFramebuffersEXT =(PFNDeleteFramebuffersEXT ) glXGetProcAddressARB((const GLubyte *) "glDeleteFramebuffersEXT");
glGenFramebuffersEXT =(PFNGenFramebuffersEXT ) glXGetProcAddressARB((const GLubyte *) "glGenFramebuffersEXT");
glFramebufferTexture2DEXT =(PFNFramebufferTexture2DEXT ) glXGetProcAddressARB((const GLubyte *) "glFramebufferTexture2DEXT");
glActiveTextureARB = (PFNGLActiveTextureARB) glXGetProcAddressARB((const GLubyte *) "glActiveTextureARB");
glGenProgramsARB = (PFNGenProgramsARB) glXGetProcAddressARB((const GLubyte *) "glGenProgramsARB");
glLoadProgramARB= (PFNLoadProgramARB) glXGetProcAddressARB((const GLubyte *) "glLoadProgramARB");
glBindProgramARB = (PFNGLBindProgramARB) glXGetProcAddressARB((const GLubyte *) "glBindProgramARB");
glProgramStringARB = (PFNGLProgramStringARB) glXGetProcAddressARB((const GLubyte *) "glProgramStringARB");
glDeleteProgramsARB = (PFNGLDeleteProgramsARB) glXGetProcAddressARB((const GLubyte *)"glDeleteProgramsARB");
#endif

}

ARBFProg::~ARBFProg()
{
	glDeleteProgramsARB(1,&prog_id);
}
void ARBFProg::Load(char* prog)
{
    glEnable(GL_FRAGMENT_PROGRAM_ARB);
	glGenProgramsARB( 1, &prog_id );
   	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, prog_id);
	glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, strlen(prog), prog);
	GLenum error = glGetError();
	if( error != GL_NO_ERROR )
		fprintf( stderr, "ERROR\n%s\n", gluErrorString( error ) );
}


void ARBFProg::Bind()
{
	glEnable( GL_FRAGMENT_PROGRAM_ARB );  
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, prog_id);

	GLenum error = glGetError();
	if( error != GL_NO_ERROR )
		fprintf( stderr, "ERROR - Bind()\n%s progid: %d\n", gluErrorString( error ),prog_id );
		
}

void ARBFProg::BindProg(){
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, prog_id);
    
}

void ARBFProg::Release()
{
	glDisable( GL_FRAGMENT_PROGRAM_ARB );  
	GLenum error = glGetError();
	if( error != GL_NO_ERROR )
		fprintf( stderr, "ERROR - Release()\n%s\n", gluErrorString( error ) );

}

void ARBFProg::SetConstant(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,index,x,y,z,w);
}

