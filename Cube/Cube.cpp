// Jeremy Vercillo
// 2/15/12

#include <string>
#include <string.h>
#include <stdio.h>
#include <glm/glm.hpp>
#include "questionblock_texture.c"
#include "brickblock_texture.c"
#include "groundblock_texture.c"
#include "skybox_texture.c"
#include "title_texture.c"
#include "../common/shader_utils.h"
#include "Cube.h"

Cube::Cube(float x, float y, float z, const char* texture, float s) {
	position = glm::vec3(x, y, z);
	velocity = glm::vec3(0.0, 0.0, 0.0);
	bool destroyed = false;
	bool hit = false;
	
//	GLfloat specrefon[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//	GLfloat sprcrefoff[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	size = s;
	texturename = texture;
    //setReflect(0);

	GLfloat tempverts[72] = {
		// front face
		-(size/2), -(size/2),  (size/2),
		 (size/2), -(size/2),  (size/2),
		 (size/2),  (size/2),  (size/2),
		-(size/2),  (size/2),  (size/2),
		// top face
		-(size/2),  (size/2),  (size/2),
		 (size/2),  (size/2),  (size/2),
		 (size/2),  (size/2), -(size/2),
		-(size/2),  (size/2), -(size/2),
		// back face
		 (size/2), -(size/2), -(size/2),
		-(size/2), -(size/2), -(size/2),
		-(size/2),  (size/2), -(size/2),
		 (size/2),  (size/2), -(size/2),
		// bottom face
		-(size/2), -(size/2), -(size/2),
		 (size/2), -(size/2), -(size/2),
		 (size/2), -(size/2),  (size/2),
		-(size/2), -(size/2),  (size/2),
		// left face
		-(size/2), -(size/2), -(size/2),
		-(size/2), -(size/2),  (size/2),
		-(size/2),  (size/2),  (size/2),
		-(size/2),  (size/2), -(size/2),
		// right face
		 (size/2), -(size/2),  (size/2),
		 (size/2), -(size/2), -(size/2),
		 (size/2),  (size/2), -(size/2),
		 (size/2),  (size/2),  (size/2),
	};
	for(int n = 0; n < 72; n++) vertices[n] = tempverts[n];
	glGenBuffers(1, &vbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// binds the vertex positions of the cube
	
	if(strcmp(texture,"questionblock") == 0) {
		GLfloat tempcoords[48] = {
			0.0, 0.0,
			0.5, 0.0,
			0.5, 1.0,
			0.0, 1.0,
		}; // what part of the texture image is used
		GLfloat texcoords_tops[8] = {
			0.5, 0.0,
			1.0, 0.0,
			1.0, 1.0,
			0.5, 1.0,
		}; // the other part of the texture
		for(int n = 0; n < 8; n++) texcoords[n] = tempcoords[n];
		for(int n = 1; n < 6; n++) memcpy(&texcoords[n*4*2], &texcoords[0], 2*4*sizeof(GLfloat));
		memcpy(&texcoords[1*4*2], &texcoords_tops[0], 2*4*sizeof(GLfloat)); // top
		memcpy(&texcoords[3*4*2], &texcoords_tops[0], 2*4*sizeof(GLfloat)); // bottom
		glGenBuffers(1, &vbo_texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// binds texture coordinates
		
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 
			0,
			GL_RGB,
			questionblock_texture.width,
			questionblock_texture.height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			questionblock_texture.pixel_data);	
			// texture object
	}
	else if(strcmp(texture,"brickblock") == 0) {
		GLfloat tempcoords[48] = {
			0.0, 0.0,
			1.0, 0.0,
			1.0, 1.0,
			0.0, 1.0,
		}; // what part of the texture image is used
		for(int n = 0 ; n < 8; n++) texcoords[n] = tempcoords[n];
		for(int n = 1; n < 6; n++) memcpy(&texcoords[n*4*2], &texcoords[0], 2*4*sizeof(GLfloat));
		glGenBuffers(1, &vbo_texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// binds texture coordinates
		
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 
			0,
			GL_RGB,
			brickblock_texture.width,
			brickblock_texture.height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			brickblock_texture.pixel_data);
			// texture object
	}
	else if(strcmp(texture,"groundblock") == 0) {
		GLfloat tempcoords[48] = {
			0.0 + .01, 0.0 + .01,
			1.0 - .01, 0.0 + .01,
			1.0 - .01, 1.0 - .01,
			0.0 + .01, 1.0 - .01,
		}; // what part of the texture image is used
		for(int n = 0 ; n < 8; n++) texcoords[n] = tempcoords[n];
		for(int n = 1; n < 6; n++) memcpy(&texcoords[n*4*2], &texcoords[0], 2*4*sizeof(GLfloat));
		glGenBuffers(1, &vbo_texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// binds texture coordinates
		
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 
			0,
			GL_RGB,
			groundblock_texture.width,
			groundblock_texture.height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			groundblock_texture.pixel_data);
			// texture object
	}
	else if(strcmp(texture,"skybox") == 0) {
		GLfloat frontcoords[8] = {
			0.25 + .0015, 1.0/3.0 + .0015,
			0.5  - .0015, 1.0/3.0 + .0015,
			0.5  - .0015, 2.0/3.0 - .0015,
			0.25 + .0015, 2.0/3.0 - .0015,
		}; // front
		GLfloat topcoords[8] = {
			0.25 + .0015, 2.0/3.0 + .0015,
			0.5  - .0015, 2.0/3.0 + .0015,
			0.5  - .0015, 1.0 - .0015,
			0.25 + .0015, 1.0 - .0015,		
		}; // top
		GLfloat backcoords[8] = {
			0.75 + .0015, 1.0/3.0 + .0015,
			1.0  - .0015, 1.0/3.0 + .0015,
			1.0  - .0015, 2.0/3.0 - .0015,
			0.75 + .0015, 2.0/3.0 - .0015,
		}; // back
		GLfloat bottomcoords[8] = {
			0.25 + .0015, 0.0 + .0015,
			0.5  - .0015, 0.0 + .0015,
			0.5  - .0015, 1.0/3.0 - .0015,
			0.25 + .0015, 1.0/3.0 - .0015,	
		}; // bottom
		GLfloat leftcoords[8] = {
			0.0  + .0015, 1.0/3.0 + .0015,
			0.25 - .0015, 1.0/3.0 + .0015,
			0.25 - .0015, 2.0/3.0 - .0015,
			0.0  + .0015, 2.0/3.0 - .0015,
		}; // left
		GLfloat rightcoords[8] = {
			0.5  + .0015, 1.0/3.0 + .0015,
			0.75 - .0015, 1.0/3.0 + .0015,
			0.75 - .0015, 2.0/3.0 - .0015,
			0.5  + .0015, 2.0/3.0 - .0015,
		}; // right
		for(int n = 0; n < 8; n++) texcoords[n] = frontcoords[n];
		memcpy(&texcoords[1*2*4], &topcoords, 2*4*sizeof(GLfloat));
		memcpy(&texcoords[2*2*4], &backcoords, 2*4*sizeof(GLfloat));
		memcpy(&texcoords[3*2*4], &bottomcoords, 2*4*sizeof(GLfloat));
		memcpy(&texcoords[4*2*4], &leftcoords, 2*4*sizeof(GLfloat));
		memcpy(&texcoords[5*2*4], &rightcoords, 2*4*sizeof(GLfloat));
		glGenBuffers(1, &vbo_texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGB,
			skybox_texture.width,
			skybox_texture.height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			skybox_texture.pixel_data);
			// texture object
	}
	else if(strcmp(texture, "title") == 0) {
		GLfloat tempcoords[48] = {
			0.0, 0.0,
			1.0, 0.0,
			1.0, 1.0,
			0.0, 1.0,
		}; // what part of the texture image is used
		for(int n = 0 ; n < 8; n++) texcoords[n] = tempcoords[n];
		for(int n = 1; n < 6; n++) memcpy(&texcoords[n*4*2], &texcoords[0], 2*4*sizeof(GLfloat));
		glGenBuffers(1, &vbo_texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// binds texture coordinates
		
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 
			0,
			GL_RGB,
			title_texture.width,
			title_texture.height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			title_texture.pixel_data);
			// texture object
	}
	else fprintf(stderr, "Texture %s not found.\n", texture);
		
	GLshort tempelements[36] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// top
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,
		// bottom
		12, 13, 14,
		14, 15, 12,
		// left
		16, 17, 18,
		18, 19, 16,
		// right
		20, 21, 22,
		22, 23, 20,
	};
	for(int n = 0; n < 36; n++) elements[n] = tempelements[n];
	glGenBuffers(1, &ibo_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
} // creates cube

/*void Cube::setReflect(int reflect) {
    switch (reflect) {
        case 1: 
            glMaterialfv(GL_FRONT, GL_SPECULAR, specrefon);
            glMateriali(GL_FRONT, GL_SHININESS, 128);
            break;
        case 0:
            glMaterialfv(GL_FRONT, GL_SPECULAR, specrefoff);
            break;
    }
}*/ // 1 sets reflection to full, 0 turns off

bool Cube::intersectsWith(float x, float y, float z) {
	return x >= position.x-size/2 && x <= position.x+size/2
		&& y >= position.y-size/2 && y <= position.y+size/2
		&& z >= position.z-size/2 && z <= position.z+size/2;
} // whether the point (x, y, z) is in the cube (collides with it)

bool Cube::collidesX(Cube* other) {
	glm::vec3 nextpos = position + velocity;
	glm::vec3 othernextpos = other->position + other->velocity;
	return abs(position.x-other->position.x) > (size/2 + other->size/2)
		&& abs(nextpos.x-othernextpos.x) <= (size/2+other->size/2)
		&& abs(nextpos.y-othernextpos.y) <= (size/2+other->size/2)
		&& abs(nextpos.z-othernextpos.z) <= (size/2+other->size/2);
}

bool Cube::collidesY(Cube* other) {
	glm::vec3 nextpos = position + velocity;
	glm::vec3 othernextpos = other->position + other->velocity;
	return abs(position.y-other->position.y) > (size/2 + other->size/2)
		&& abs(nextpos.y-othernextpos.y) <= (size/2+other->size/2)
		&& abs(nextpos.x-othernextpos.x) <= (size/2+other->size/2)
		&& abs(nextpos.z-othernextpos.z) <= (size/2+other->size/2);
}

bool Cube::collidesTopY(Cube* other) {
	glm::vec3 nextpos = position + velocity;
	glm::vec3 othernextpos = other->position + other->velocity;
    return position.y > other->position.y+size && position.y <= other->position.y+(other->size+size)
		&& abs(nextpos.x-othernextpos.x) <= (size/2+other->size/2)
		&& abs(nextpos.z-othernextpos.z) <= (size/2+other->size/2);
}

bool Cube::collidesBottomY(Cube* other) {
	glm::vec3 nextpos = position + velocity;
	glm::vec3 othernextpos = other->position + other->velocity;
    return position.y <= other->position.y && position.y >= other->position.y-(size+other->size)
		&& abs(nextpos.x-othernextpos.x) <= (size/2+other->size/2)
		&& abs(nextpos.z-othernextpos.z) <= (size/2+other->size/2);
}

bool Cube::collidesZ(Cube* other) {
	glm::vec3 nextpos = position + velocity;
	glm::vec3 othernextpos = other->position + other->velocity;
	return abs(position.z-other->position.z) > (size/2 + other->size/2)
		&& abs(nextpos.z-othernextpos.z) <= (size/2+other->size/2)
		&& abs(nextpos.x-othernextpos.x) <= (size/2+other->size/2)
		&& abs(nextpos.y-othernextpos.y) <= (size/2+other->size/2);
}

bool Cube::collidesWith(Cube* other) {
	return collidesX(other) || collidesY(other) || collidesZ(other);
} // whether the point (x, y, z) is in the cube (collides with it)

void Cube::move(float x, float y, float z) {
	position = glm::vec3(x, y, z);
}

void Cube::printPos() {
	float x,y,z;
	x = position.x;
	y = position.y;
	z = position.z;
	printf("(%f, %f, %f)\n",x,y,z);
}

Cube::~Cube() {
	glDeleteBuffers(1, &vbo_vertices);
	glDeleteBuffers(1, &vbo_texcoords);
	glDeleteBuffers(1, &ibo_elements);
	glDeleteTextures(1, &texture_id);
} // frees resources
