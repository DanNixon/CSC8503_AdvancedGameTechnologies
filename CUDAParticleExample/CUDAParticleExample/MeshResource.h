#pragma once
#include <GL\glew.h>


enum VBOType { VBO_VERTEX, VBO_NORMAL, VBO_TEXCOORD, VBO_MAX };

class MeshResource {
public:
	MeshResource() {
		glGenVertexArrays(1, vao);
		for (int i = 0; i < VBO_MAX; i++)
			vbo[i] = 0;
		ibo[0] = 0;
		mode = GL_TRIANGLES;
	}
	virtual ~MeshResource() {
		glDeleteBuffers(VBO_MAX, vbo);
		glDeleteBuffers(1, ibo);
		glDeleteVertexArrays(1, vao);
	}

	virtual void draw() const;

protected:
	void initVBO(VBOType type, float* data, int comps, int num, unsigned int mode);
	void initIBO(unsigned int* data, int num, unsigned int mode);

	unsigned int vao[1];
	unsigned int vbo[VBO_MAX];
	unsigned int ibo[1];
	unsigned int numIndices;

	unsigned int mode;
};