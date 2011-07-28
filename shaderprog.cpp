#include <stdio.h>

#include "shaderprog.h"

ShaderProg::~ShaderProg()
{
	glDeleteProgram(prog_id);
}
void ShaderProg::Load(char* vprog, char* fprog)
{
    strncpy(source, fprog, 1024);
    
    prog_id = glCreateProgram();

	GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint vshader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(fshader, 1 , (const char **) &fprog, 0);
	glShaderSource(vshader, 1 , (const char **) &vprog, 0);
	
	glCompileShader(fshader);
	glCompileShader(vshader);
	
	GLint e;
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &e);
	if (!e) {
		fprintf(stderr, "Error compiling vertex shader:\n");
		int len;
		char log[1024];
		glGetShaderInfoLog(vshader, 1024, &len, log);
		fprintf(stderr, "%s\n", log);
	}	 

	glGetShaderiv(fshader, GL_COMPILE_STATUS, &e);
	if (!e) {
		printf("Error compiling fragment shader:\n%s", fprog);
		int len;
		char log[1024];
		glGetShaderInfoLog(fshader, 1024, &len, log);
		fprintf(stderr, "%s\n", log);
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
		fprintf(stderr, "%s\n", log);
	}	 

	GLenum error = glGetError();
	if( error != GL_NO_ERROR )
		fprintf( stderr, "Program compile error: \n0x%x\n", error );
}


void ShaderProg::Bind()
{  
  	GLenum error = glGetError();
	if( error != GL_NO_ERROR ){
	  fprintf( stderr, "ERROR - Pre-Bind()\n0x%x progid: %d\n", error, prog_id );
	}

	glUseProgram( prog_id );

	if( error != GL_NO_ERROR ){
	  fprintf( stderr, "ERROR - Bind()\n0x%x progid: %d\n", error, prog_id );
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

void ShaderProg::SetConstant(const char* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	GLuint index = glGetUniformLocation( prog_id, name );
	glUniform4f(index,x,y,z,w);

}

