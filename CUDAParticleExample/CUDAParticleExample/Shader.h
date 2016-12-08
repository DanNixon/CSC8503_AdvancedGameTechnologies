#pragma once
#include <map>
#include <string>
#include <GL\glew.h>
#include "Matrix4.h"

class Shader {
public:
	Shader(): prog(0) {}
	Shader(const char* vertSrc, const char* fragSrc);
	~Shader();
	
	void use();

	void bindAttributes();
	void send(int loc, int number, Matrix4* mat);

	unsigned int getProgram() const { return prog; }

private:
	unsigned int compileShader(const char* src, GLenum type);
	void linkProgram(unsigned int vs, unsigned int fs);

	unsigned int prog;
};