// Jeremy Vercillo
// 2/9/12

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLUT/glut.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
// libraries

#include "Cube.h"
#include "../common/shader_utils.h"
// local includes

#define PI 3.1415926

GLuint program;
GLint attribute_coord3d, attribute_texcoord;
GLint uniform_mvp;

int screen_width = 800, screen_height = 600; // screen size
int fullscreen = 0;

int keys[256] = {0}; // array of whether keys are pressed

int cubesize = 2;
int numcubes = 100;
Cube* cubes[100]; // array of cubes
Cube* bg; // background skycube

float eyedist = 20.0; // distance away
float eyephi = 180; // degrees sideways
float eyetheta = 30; // degrees up
// location of the camera

float camspeed = .5;
float zoomspeed = .1;
// movemevt speeds

float lastframe = 0; // last frame in ms from GLUT_ELAPSED_TIME
float MAX_FPS = 60.0; // 60 frames per second

int initShaders() {
	GLuint vs, fs;
	if((vs = create_shader("cubeshader.v.glsl", GL_VERTEX_SHADER)) == 0) return 0;
	if((fs = create_shader("cubeshader.f.glsl", GL_FRAGMENT_SHADER)) == 0) return 0;
	// creates shaders from my files
	
	GLint link_ok = GL_FALSE;
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if(!link_ok) {
		fprintf(stderr, "glLinkProgram:");
		return 0;
	}
	// checks linking of shaders
	
	const char* attribute_name = "coord3d";
	attribute_coord3d = glGetAttribLocation(program, attribute_name);
	if(attribute_coord3d == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return 0;
	}
	attribute_name = "texcoord";
	attribute_texcoord = glGetAttribLocation(program, attribute_name);
	if(attribute_texcoord == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return 0;
	}
	// checks binding of attributes
	
	const char* uniform_name;
	uniform_name = "mvp";
	uniform_mvp = glGetUniformLocation(program, uniform_name);
	if(uniform_mvp == -1) {
		fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
		return 0;
	}
	
	return 1;
}

float distance(float x1, float y1, float z1, float x2, float y2, float z2) {
	return sqrt((x2*x2-x1*x1) + (y2*y2-y1*y1) + (z2*z2-z1*z1));
} // distance between 2 points

void moveCamera() {
	if(keys['w']) {
		eyetheta += camspeed;
		if(eyetheta > 180) eyetheta -= 360;
	} // up
	if(keys['a']) {
		eyephi -= camspeed;
		if(eyephi < -180) eyephi += 360;
	} // left
	if(keys['s']) {
		eyetheta -= camspeed;
		if(eyetheta < -180) eyetheta += 360;
	} // down
	if(keys['d']) {
		eyephi += camspeed;
		if(eyephi > 180) eyephi -= 360;
	} // right
	if(keys['r']) {
		eyedist -= zoomspeed;
		if(eyedist <= 0) eyedist = 0;
	} // zoom in
	if(keys['f']) {
		eyedist += zoomspeed;
	} // zoom out
} // moves camera based on current key states

void idle() {
	moveCamera();
	glutPostRedisplay();
} // constantly calculates redraws

void onDisplay() {
	glClearColor(0.0, 0.0, 0.0, 0.0); // black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// clears the screen

	glUseProgram(program);

	float lookx = cubes[0]->xpos;
	float looky = cubes[0]->ypos;
	float lookz = cubes[0]->zpos;
	float eyex = lookx + eyedist*cos(eyetheta*PI/180.0)*cos(eyephi*PI/180.0);
	float eyey = looky + eyedist*sin(eyetheta*PI/180.0);
	float eyez = lookz + eyedist*cos(eyetheta*PI/180.0)*sin(eyephi*PI/180.0);	
	float up = 1.0;
	if(abs(eyetheta) > 90.0) up = -1.0;
	// eye and look coordinates are the same for all cubes
	
	glm::mat4 view = glm::lookAt(glm::vec3(eyex, eyey, eyez), glm::vec3(lookx, looky, lookz), glm::vec3(0.0, up, 0.0));
	glm::mat4 projection = glm::perspective(
			45.0f, // view angle (y direction)
			1.0f*screen_width/screen_height, // aspect ratio
			0.1f, // near clipping plane
			5000.0f); // far clipping plane
	// projection from the eye	


	// draws background (skybox)
	glBindTexture(GL_TEXTURE_2D, bg->texture_id);
	
	glEnableVertexAttribArray(attribute_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, bg->vbo_texcoords);
	glVertexAttribPointer(
		attribute_texcoord,			// attrib name (in v shader)
		2,							// num elements per vertex (x, y)
		GL_FLOAT,					// element type
		GL_FALSE,					// take values as-is
		0,							// no stride
		0);							// no first element offset

	glEnableVertexAttribArray(attribute_coord3d);
	glBindBuffer(GL_ARRAY_BUFFER, bg->vbo_vertices);
	glVertexAttribPointer(
		attribute_coord3d,			// attrib name (in v shader)
		3,							// num elements per vertex (x, y, z)
		GL_FLOAT,					// element type
		GL_FALSE,					// take values as-is
		0,							// no stride
		0);							// first element offset

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bg->ibo_elements);
	int size; glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(bg->xpos, bg->ypos, bg->zpos));
	// translate to position from origin
	glm::mat4 mvp = projection * view * model;	
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	for(int n = 0; n < numcubes; n++) {
		glBindTexture(GL_TEXTURE_2D, cubes[n]->texture_id);
	
		glEnableVertexAttribArray(attribute_texcoord);
		glBindBuffer(GL_ARRAY_BUFFER, cubes[n]->vbo_texcoords);
		glVertexAttribPointer(
			attribute_texcoord,			// attrib name (in v shader)
			2,							// num elements per vertex (x, y)
			GL_FLOAT,					// element type
			GL_FALSE,					// take values as-is
			0,							// no stride
			0);							// no first element offset

		glEnableVertexAttribArray(attribute_coord3d);
		glBindBuffer(GL_ARRAY_BUFFER, cubes[n]->vbo_vertices);
		glVertexAttribPointer(
			attribute_coord3d,			// attrib name (in v shader)
			3,							// num elements per vertex (x, y, z)
			GL_FLOAT,					// element type
			GL_FALSE,					// take values as-is
			0,							// no stride
			0);							// first element offset

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubes[n]->ibo_elements);
		int size; glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(cubes[n]->xpos, cubes[n]->ypos, cubes[n]->zpos));
		// translate to position from origin
		glm::mat4 mvp = projection * view * model;	
		glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
		glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	} // places and draws all cubes

	glDisableVertexAttribArray(attribute_coord3d);
	glDisableVertexAttribArray(attribute_texcoord);
	// disables attribute arrays
	
	while((glutGet(GLUT_ELAPSED_TIME) - lastframe) < 1000.0/MAX_FPS) {
		continue;
	} // limits FPS
	printf("%.2f FPS\n", 1000.0/(glutGet(GLUT_ELAPSED_TIME) - lastframe));
	lastframe = glutGet(GLUT_ELAPSED_TIME);

	glutSwapBuffers();
} // displays to screen

void reshape(int width, int height) {
	screen_width = width;
	screen_height = height;
	glViewport(0, 0, screen_width, screen_height);
} // resizes screen

void free_resources() {
	glDeleteProgram(program);
	for(int n = 0; n < numcubes; n++) {
		delete cubes[n];
	}
} // cleans up memory

void key_pressed(unsigned char key, int x, int y) {
	keys[key] = 1; // key is pressed
	if(key == 'm' && !fullscreen) {
		glutFullScreen();
		fullscreen = 1;
	}
	else if(key == 'm' && fullscreen) {
		glutReshapeWindow(800, 600);
		glutPositionWindow(0, 0);
		fullscreen = 0;
	}
} // watches keyboard

void key_released(unsigned char key, int x, int y) {
	keys[key] = 0; // key is no longer pressed
} // watches keyboard

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(screen_width, screen_height);
	glutCreateWindow("My Rotating Cube");
	// initializes glut window

	GLenum glew_status = glewInit();
	if(glew_status != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		return 1;
	}
	if (!GL_VERSION_2_0) {
		fprintf(stderr, "Error: your graphic card does not support OpenGL 2.0\n");
		return 1;
	}
	// initializes GLEW and checks for errors

	bg = new Cube(0.0, 0.0, 0.0, "skybox", 3000);
	for(int n = 0; n < numcubes; n++) {
		cubes[n] = new Cube(cubesize*n, 0.0, -cubesize, (n%2==0)?("brickblock"):("questionblock"), cubesize);
	}

	if(initShaders()) {
		glutDisplayFunc(onDisplay);
		glutReshapeFunc(reshape);
		glutIdleFunc(idle);
		glutKeyboardFunc(key_pressed);
		glutKeyboardUpFunc(key_released);
		// glut functions
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glutMainLoop();
	}
	
	free_resources();
	return 0;
}
