// Jeremy Vercillo
// 2/10/12

#ifndef _CREATE_SHADER_H
#define _CREATE_SHADER_H

#include <GL/glew.h>
//#include <GL/glut.h>
//#include <GL/gl.h>
//#include <GL/glu.h>

char* file_read(const char* filename);
void print_log(GLuint object);
GLuint create_shader(const char* filename, GLenum type);

#endif
