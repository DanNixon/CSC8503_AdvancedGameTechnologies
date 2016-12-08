#include "MeshResource.h"

void MeshResource::initVBO(VBOType type, float* data, int comps, int num, unsigned int mode) {
	glGenBuffers(1, &vbo[type]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[type]);
	glBufferData(GL_ARRAY_BUFFER, num*comps*sizeof(GLfloat), (GLvoid*)data, mode);
	glVertexAttribPointer((GLuint)type, comps, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray((GLuint)type);
}

void MeshResource::initIBO(unsigned int* data, int num, unsigned int mode) {
	numIndices = num;
	glGenBuffers(1, ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, num * sizeof(GLuint), (GLvoid*)data, mode);
}

void MeshResource::draw() const {
	glBindVertexArray(vao[0]);
	glDrawElements(mode, numIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}