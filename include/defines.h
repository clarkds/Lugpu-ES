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
#ifndef __DEFINES_H_
#define __DEFINES_H_

#ifdef WIN32
#include <windows.h>
#else
#include <GL/glx.h>
#endif

typedef bool (APIENTRY * PFNIsRenderbufferEXT)(unsigned int renderbuffer);
typedef void (APIENTRY * PFNBindRenderbufferEXT)(int target, unsigned int renderbuffer);
typedef void (APIENTRY * PFNDeleteRenderbuffersEXT)(GLsizei n, const unsigned int *renderbuffers);
typedef void (APIENTRY * PFNGenRenderbuffersEXT)(GLsizei n, unsigned int *renderbuffers);

typedef void (APIENTRY * PFNRenderbufferStorageEXT)(int target, int internalformat,
							GLsizei width, GLsizei height);

typedef void (APIENTRY * PFNGetRenderbufferParameterivEXT)(int target, int pname, int* params);

typedef bool (APIENTRY * PFNIsFramebufferEXT)(unsigned int framebuffer);
typedef void (APIENTRY * PFNBindFramebufferEXT)(int target, unsigned int framebuffer);
typedef void (APIENTRY * PFNDeleteFramebuffersEXT)(GLsizei n, const unsigned int *framebuffers);
typedef void (APIENTRY * PFNGenFramebuffersEXT)(GLsizei n, unsigned int *framebuffers);
typedef void (APIENTRY * PFNFramebufferTexture1DEXT)(int target, int attachment,
							 int textarget, unsigned int texture,
							 int level);
typedef void (APIENTRY * PFNFramebufferTexture2DEXT)(int target, int attachment,
							 int textarget, unsigned int texture,
							 int level);
typedef void (APIENTRY * PFNFramebufferTexture3DEXT)(int target, int attachment,
							 int textarget, unsigned int texture,
							 int level, int zoffset);

typedef void (APIENTRY * PFNFramebufferRenderbufferEXT)(int target, int attachment,
								int renderbuffertarget, unsigned int renderbuffer);

typedef void (APIENTRY * PFNGetFramebufferAttachmentParameterivEXT)(unsigned int target, int attachment,
											int pname, int *params);

typedef void (APIENTRY * PFNGenerateMipmapEXT)(unsigned int target);

typedef void (APIENTRY * PFNGLBINDBUFFERARBPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRY * PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRY * PFNGLGENBUFFERSARBPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRY * PFNGLBUFFERDATAARBPROC) (GLenum target, int size, const GLvoid *data, GLenum usage);

typedef void (APIENTRY * PFNGLGENOCCLUSIONQUERIESNVPROC) (GLsizei n, GLuint *ids);
typedef void (APIENTRY * PFNGLDELETEOCCLUSIONQUERIESNVPROC) (GLsizei n, const GLuint *ids);
typedef GLboolean (APIENTRY * PFNGLISOCCLUSIONQUERYNVPROC) (GLuint id);
typedef void (APIENTRY * PFNGLBEGINOCCLUSIONQUERYNVPROC) (GLuint id);
typedef void (APIENTRY * PFNGLENDOCCLUSIONQUERYNVPROC) (void);
typedef void (APIENTRY * PFNGLGETOCCLUSIONQUERYIVNVPROC) (GLuint id, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETOCCLUSIONQUERYUIVNVPROC) (GLuint id, GLenum pname, GLuint *params);

typedef void (APIENTRY* PFNGLPROGRAMLOCALPARAMETER4FARBPROC) (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);



extern void APIENTRY glMultiDrawArraysEXT (GLenum, GLint *, GLsizei *, GLsizei);
extern void APIENTRY glMultiDrawElementsEXT (GLenum, const GLsizei *, GLenum, const GLvoid* *, GLsizei);


extern PFNGLGENBUFFERSARBPROC glGenBuffersARB ;					// VBO Name Generation Procedure
extern PFNGLBINDBUFFERARBPROC glBindBufferARB ;					// VBO Bind Procedure
extern PFNGLBUFFERDATAARBPROC glBufferDataARB ;					// VBO Data Loading Procedure
extern PFNGLDELETEBUFFERSARBPROC glDeleteBuffers ;			// VBO Deletion Procedure


typedef GLvoid    (APIENTRY * PFNGLGENOCCLUSIONQUERIESNVPROC) (GLsizei n, GLuint *ids);
typedef GLvoid    (APIENTRY * PFNGLDELETEOCCLUSIONQUERIESNVPROC) (GLsizei n, const GLuint *ids);
typedef GLboolean (APIENTRY * PFNGLISOCCLUSIONQUERYNVPROC) (GLuint id);
typedef GLvoid    (APIENTRY * PFNGLBEGINOCCLUSIONQUERYNVPROC) (GLuint id);
typedef GLvoid    (APIENTRY * PFNGLENDOCCLUSIONQUERYNVPROC) (GLvoid);
typedef GLvoid    (APIENTRY * PFNGLGETOCCLUSIONQUERYIVNVPROC) (GLuint id, GLenum pname, GLint *params);
typedef GLvoid    (APIENTRY * PFNGLGETOCCLUSIONQUERYUIVNVPROC) (GLuint id, GLenum pname, GLuint *params);

extern PFNGLGENOCCLUSIONQUERIESNVPROC glGenOcclusionQueriesNV;
extern PFNGLDELETEOCCLUSIONQUERIESNVPROC glDeleteOcclusionQueriesNV ;
extern PFNGLISOCCLUSIONQUERYNVPROC glIsOcclusionQueryNV ;
extern PFNGLBEGINOCCLUSIONQUERYNVPROC glBeginOcclusionQueryNV ;
extern PFNGLENDOCCLUSIONQUERYNVPROC glEndOcclusionQueryNV ;
extern PFNGLGETOCCLUSIONQUERYIVNVPROC glGetOcclusionQueryivNV ;
extern PFNGLGETOCCLUSIONQUERYUIVNVPROC glGetOcclusionQueryuivNV ;

extern PFNGLPROGRAMLOCALPARAMETER4FARBPROC glProgramLocalParameter4fARB;


#define WGL_FRONT_LEFT_ARB 0x2083
#define WGL_BACK_LEFT_ARB 0x2085

#define GL_TEXTURE0_ARB 0x84C0
#define GL_TEXTURE1_ARB 0x84C1
#define GL_TEXTURE2_ARB 0x84C2
#define GL_TEXTURE3_ARB 0x84C3
#define GL_TEXTURE4_ARB 0x84C4
#define GL_TEXTURE5_ARB 0x84C5
#define GL_TEXTURE6_ARB 0x84C6
#define GL_TEXTURE7_ARB 0x84C7
#define GL_TEXTURE8_ARB 0x84C8
#define GL_TEXTURE9_ARB 0x84C9
#define GL_TEXTURE_RECTANGLE_NV 0x84F5
#define GL_PROXY_TEXTURE_RECTANGLE_NV 0x84F7 
#define GL_MAX_RECTANGLE_TEXTURE_SIZE_NV 0x84F8
#define GL_FLOAT_RGBA16_NV 0x888A
#define GL_FLOAT_RGBA32_NV 0x888B
#define GL_ARRAY_BUFFER_ARB 0x8892
#define GL_STATIC_DRAW_ARB 0x88E4
#define GL_FLOAT_R32_NV 0x8885


#define			GL_PIXEL_COUNTER_BITS_NV				0x8864
#define			GL_CURRENT_OCCLUSION_QUERY_ID_NV		0x8865
#define			GL_PIXEL_COUNT_NV						0x8866
#define			GL_PIXEL_COUNT_AVAILABLE_NV				0x8867


#define		    GL_FRAMEBUFFER_EXT						0x8D40
#define			GL_COLOR_ATTACHMENT0_EXT                0x8CE0
#define			GL_COLOR_ATTACHMENT1_EXT                0x8CE1
#define         GL_COLOR_ATTACHMENT2_EXT                0x8CE2
#define         GL_COLOR_ATTACHMENT3_EXT                0x8CE3
#define         GL_COLOR_ATTACHMENT4_EXT                0x8CE4
#define         GL_COLOR_ATTACHMENT5_EXT                0x8CE5
#define         GL_COLOR_ATTACHMENT6_EXT                0x8CE6
#define         GL_COLOR_ATTACHMENT7_EXT                0x8CE7
#define         GL_COLOR_ATTACHMENT8_EXT                0x8CE8
#define         GL_COLOR_ATTACHMENT9_EXT                0x8CE9
#define         GL_COLOR_ATTACHMENT10_EXT               0x8CEA
#define         GL_COLOR_ATTACHMENT11_EXT               0x8CEB
#define         GL_COLOR_ATTACHMENT12_EXT               0x8CEC
#define         GL_COLOR_ATTACHMENT13_EXT               0x8CED
#define         GL_COLOR_ATTACHMENT14_EXT               0x8CEE
#define         GL_COLOR_ATTACHMENT15_EXT               0x8CEF
#define         GL_DEPTH_ATTACHMENT_EXT                 0x8D00
#define         GL_STENCIL_ATTACHMENT_EXT               0x8D20
#define			GL_MAX_TEXTURE_UNITS_ARB				0x84E2
#define 		GL_TEXTURE_3D							0x806F

typedef void (APIENTRY * PFNGLActiveTextureARB) (GLint texture);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FPROC) (GLenum target, GLfloat s, GLfloat t);
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FVPROC) (GLenum target, const GLfloat *v);

extern PFNGLMULTITEXCOORD2FPROC			glMultiTexCoord2f;
extern PFNGLMULTITEXCOORD2FVPROC			glMultiTexCoord2fv;

extern PFNIsRenderbufferEXT glIsRenderbufferEXT;
extern PFNBindRenderbufferEXT glBindRenderbufferEXT;
extern PFNDeleteRenderbuffersEXT glDeleteRenderbuffersEXT;
extern PFNGenRenderbuffersEXT glGenRenderbuffersEXT;
extern PFNRenderbufferStorageEXT glRenderbufferStorageEXT;
extern PFNGetRenderbufferParameterivEXT glGetRenderbufferParameterivEXT;
extern PFNIsFramebufferEXT glIsFramebufferEXT;
extern PFNBindFramebufferEXT glBindFramebufferEXT;
extern PFNDeleteFramebuffersEXT glDeleteFramebuffersEXT;
extern PFNGenFramebuffersEXT glGenFramebuffersEXT;
extern PFNFramebufferTexture2DEXT glFramebufferTexture2DEXT;


extern PFNGLActiveTextureARB glActiveTextureARB;



void InitExts();

#endif
