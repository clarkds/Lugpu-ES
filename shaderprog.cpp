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

#include "shaderprog.h"

ShaderProg::~ShaderProg()
{
	glDeleteProgram(prog_id);
}
void ShaderProg::Load(char* vprog, char* fprog)
{
  memcpy(source,fprog,strlen(fprog));
    prog_id = glCreateProgram();

	GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint vshader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(fshader, 1 , (const GLchar **) &fprog, 0);
	glShaderSource(vshader, 1 , (const GLchar **) &vprog, 0);
	
	glCompileShader(fshader);
	glCompileShader(vshader);
	
	GLint e;
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &e);
	if (!e) {
		fprintf(stderr, "Error compiling vertex shader:\n");
		int len;
		char log[1024];
		glGetShaderInfoLog(vshader, 1024, &len, log);
		printf("%s\n", log);
	}	 

	glGetShaderiv(fshader, GL_COMPILE_STATUS, &e);
	if (!e) {
		printf("Error compiling fragment shader:\n%s", fprog);
		int len;
		char log[1024];
		glGetShaderInfoLog(fshader, 1024, &len, log);
		printf("%s\n", log);
	}	 

	glAttachShader( prog_id, vshader );
	glAttachShader( prog_id, fshader );

	glLinkProgram( prog_id );
	
	glGetProgramiv(prog_id, GL_LINK_STATUS, &e);
	if (!e) {
		printf("Error linking program:\n");
		int len;
		char log[1024];
		glGetProgramInfoLog(prog_id, 1024, &len, log);
		printf("%s\n", log);
	}	 

	GLenum error = glGetError();
	if( error != GL_NO_ERROR )
		fprintf( stderr, "ERROR\n0x%x\n", error );
}


void ShaderProg::Bind()
{  
	glUseProgram( prog_id );

	GLenum error = glGetError();
	if( error != GL_NO_ERROR ){
	  fprintf( stderr, "ERROR - Bind()\n0x%x progid: %d\n", error, prog_id );
	  fprintf( stderr, "Error source code : %s \n" , source);
	}
		
}

void ShaderProg::BindProg(){
	glUseProgram( prog_id );
    
}

void ShaderProg::Release()
{
	glDeleteProgram( prog_id );
	GLenum error = glGetError();
	if( error != GL_NO_ERROR )
		fprintf( stderr, "ERROR - Release()\n0x%x\n", error );

}

void ShaderProg::SetConstant(char* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	GLuint index = glGetUniformLocation( prog_id, name );
	glUniform4f(index,x,y,z,w);

}

