#include <cstdlib>
#include <cstdio>
#include "MeshResource.h"
#include "Shader.h"

Shader::Shader(const char* vertSrc, const char* fragSrc) {

	int vs = compileShader(vertSrc, GL_VERTEX_SHADER);
	int fs = compileShader(fragSrc, GL_FRAGMENT_SHADER);

	linkProgram(vs, fs);
	
	glDeleteShader(vs);
	glDeleteShader(fs);
}

Shader::~Shader() {
	glDeleteProgram(prog);
}

void Shader::use() {
	glUseProgram(prog);
}

unsigned int Shader::compileShader(const char* src, GLenum type) {

	//printf("Shader:\n%s\n", src);

	unsigned int s = glCreateShader(type);
	int len = strlen(src);
	glShaderSource(s, 1, &src, &len);
	glCompileShader(s);
	int test;
	glGetShaderiv(s, GL_COMPILE_STATUS, &test);
	if (test == GL_FALSE) {
		char infoLog[1024];
		glGetShaderInfoLog(s, 1024, NULL, infoLog);
		printf("Shader:\n%s\n", src);
		printf("Compilation Failed.\nReason:%s\n", infoLog);
		glDeleteShader(s);
		return 0;
	}
	return s;
}

void Shader::linkProgram(unsigned int vs, unsigned int fs) {
	prog = glCreateProgram();

	bindAttributes();

	glAttachShader(prog, vs);
	glAttachShader(prog, fs);

	glLinkProgram(prog);

	int test;
	glGetProgramiv(prog, GL_LINK_STATUS, &test);
	if (test == GL_FALSE) {
		char infoLog[1024];
		glGetProgramInfoLog(prog, 1024, NULL, infoLog);
		printf("Reason:%s\n", infoLog);
		glDeleteProgram(prog);
		prog = 0;
	}
}

void Shader::bindAttributes() {
	glBindAttribLocation(prog, VBO_VERTEX, "position");
	glBindAttribLocation(prog, VBO_NORMAL, "normal");
	glBindAttribLocation(prog, VBO_TEXCOORD, "texCoord");
	glBindFragDataLocation(prog, 0, "fragColour");
	glBindFragDataLocation(prog, 1, "fragNormDepth");
}

void Shader::send(int loc, int number, Matrix4* mat) {
	glUniformMatrix4fv(loc, number, false, mat->getData());
}