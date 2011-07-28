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

  void SetConstant(const char *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);

  GLuint prog_id;
  char source[1024];
};

#endif 
