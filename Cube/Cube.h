// Jeremy Vercillo
// 2/15/12
// Final Project
// A class to store an OpenGL cube

#ifndef CUBE_H
#define CUBE_H

class draw_object;

class Cube {
	public:
		glm::vec3 position, velocity;
		
		draw_object* prize;
	
//		GLfloat specrefon[];
//		GLfloat specrefoff[];
		float size;
		
		int destroycountdown;
		
		GLfloat vertices[72]; // vertecies of each face
		GLuint vbo_vertices; // VBO of coordinates
		
		GLfloat texcoords[48]; // coordinates of texture img to use
		GLuint vbo_texcoords; // VBO of texcoords
		
		GLushort elements[36]; // array indices of elements
		GLuint ibo_elements; // IBO of elements
		
		GLuint texture_id; // id code of the texture
    
       // void setReflect(int);
    
		const char* texturename;

		Cube(float, float, float, const char*, float, draw_object* = NULL);
		bool intersectsWith(float, float, float);
		bool intersectsWith(Cube*);
		bool collidesWith(Cube*, float);
		bool collidesX(Cube*, float);
		bool collidesY(Cube*, float);
		bool collidesBottomY(Cube*, float);
		bool collidesTopY(Cube*, float);
		bool collidesZ(Cube*, float);
		void move(float, float, float);
		void draw(glm::mat4, glm::mat4, GLint, GLint, GLint);
		
		bool destroyed;
		bool hit;
		void printPos();
		~Cube();
};
#endif
