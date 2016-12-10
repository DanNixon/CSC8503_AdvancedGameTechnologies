#include <iostream>

#include <GL/glew.h>
#include <minmax.h>
#include <cuda_gl_interop.h>
#include <cuda_runtime.h>

#include "MeshResource.h"
#include "Shader.h"
#include "Window.h"
#include "GameTime.h"
#include "DebugCamera.h"

using namespace std;

class VBOTestResource : public MeshResource {
public:
	VBOTestResource();
	~VBOTestResource();

	void draw() const;

	void update();
private:
	void generateGrid();

	unsigned int restart_index;
	int width, height;
	struct cudaGraphicsResource* cudaVBO;
};


VBOTestResource::VBOTestResource(): width(1000), height(1000) {

	width = max(2, width);
	height = max(2, height);
	generateGrid();

	cudaGLSetGLDevice(1);

	if (cudaGraphicsGLRegisterBuffer(&cudaVBO, vbo[VBO_VERTEX], cudaGraphicsMapFlagsNone) != cudaSuccess)
		printf("Failed\n");
}

VBOTestResource::~VBOTestResource() {
	if (cudaGraphicsUnregisterResource(cudaVBO) != cudaSuccess)
		printf("Failed\n");
}

void VBOTestResource::generateGrid() {

	int loop_size = 2*height + 1;

	int num_verts = width*height;
	int num_indices = (width - 1)*loop_size;

	float* verts = new float[3*num_verts];
	float* norms = new float[3*num_verts];
	float* texcoords = new float[2*num_verts];
	unsigned int* indices = new unsigned int[num_indices];

	mode = GL_TRIANGLE_STRIP;
	for (int x = 0; x < width; x++) {
		int loops = x*loop_size;
		for (int y = 0; y < height; y++) {
			int offset = y*width + x;

			if (x != width - 1)
				indices[loops + 2*y + 1] = offset;
			if (x != 0)
				indices[loops - loop_size + 2*y] = offset;

			verts[3*offset + 0] = 2*(x*1.0f/(width-1)) - 1;
			verts[3*offset + 1] = 0;
			verts[3*offset + 2] = 2*(y*1.0f/(height-1)) - 1;

			norms[3*offset + 0] = 0;
			norms[3*offset + 1] = 1;
			norms[3*offset + 2] = 0;

			texcoords[2*offset + 0] = x*1.0f/(width-1);
			texcoords[2*offset + 1] = y*1.0f/(height-1);
		}
		if (x != width - 1)
			indices[loops + loop_size - 1] = width*height;
	}

	restart_index = width*height;

	glBindVertexArray(vao[0]);
	initVBO(VBO_VERTEX, (float*)verts, 3, num_verts, GL_DYNAMIC_DRAW);
	initVBO(VBO_NORMAL, (float*)norms, 3, num_verts, GL_DYNAMIC_DRAW);
	initVBO(VBO_TEXCOORD, (float*)texcoords, 2, num_verts, GL_DYNAMIC_DRAW);
	initIBO(indices, num_indices, GL_DYNAMIC_DRAW);
	glBindVertexArray(0);

	delete[] verts;
	delete[] norms;
	delete[] texcoords;
	delete[] indices;
}

void VBOTestResource::draw() const {
	glPrimitiveRestartIndex(restart_index);
	glEnable(GL_PRIMITIVE_RESTART);
	MeshResource::draw();
	glDisable(GL_PRIMITIVE_RESTART);
}

__global__ void vboTestResource_update(float* ptr, int width, int height, float time) {
	int x = blockIdx.x*blockDim.x + threadIdx.x;
	int y = blockIdx.y*blockDim.y + threadIdx.y;
	int offset = y*width + x;
	if (x >= width || y >= height) return;

	float period = 30;
	float rate = 1;

	float cx = x*1.0f/width - 0.5f;
	float cy = y*1.0f/height - 0.5f;

	float wave = sin(sqrt(cx*cx + cy*cy)*period - rate*time);

	int sign = wave>0?1:-1;
	wave = sign*sqrt(sign*wave);

	ptr[3*offset + 1] = wave/10;

	period *= 3;
	rate *= -9;

	ptr[3*offset + 1] += (sin(x*period/(width - 1) + rate*time) + sin(y*period/(height - 1) + rate*time))/40;
}

void VBOTestResource::update() {
	float time = GameTime::getTime();

	float* devBuff;
	size_t size;

	dim3 threadsPerBlock(8, 8);
	dim3 numBlocks((width - 1)/threadsPerBlock.x + 1, (height - 1)/threadsPerBlock.y + 1);

	if (cudaGraphicsMapResources(1, &cudaVBO, 0) != cudaSuccess)
		printf("Failed\n");

	cudaGraphicsResourceGetMappedPointer((void**)&devBuff, &size, cudaVBO);

	vboTestResource_update<<<numBlocks, threadsPerBlock>>>(devBuff, width, height, time);

	if (cudaGraphicsUnmapResources(1, &cudaVBO, 0) != cudaSuccess)
		printf("Failed\n");
}

const char vert[] =
	"#version 150 core\n"
	"uniform mat4 model;\n"
	"uniform mat4 view;\n"
	"uniform mat4 proj;\n"
	"in vec3 position;\n"
	"in vec3 normal;\n"
	"in vec2 texCoord;\n"
	"out Vertex {\n"
	"	vec3 norm;\n"
	"	vec3 worldPos;\n"
	"	vec2 texCoord;\n"
	"} OUT;\n"
	"void main() {\n"
	"	mat4 mv = view*model;\n"
	"	vec4 worldPos = model*vec4(position, 1);\n"
	"	gl_Position = proj*view*worldPos;\n"
	"	OUT.norm = normalize(mat3(model)*normal);\n"
	"	OUT.texCoord = texCoord;\n"
	"	OUT.worldPos = worldPos.xyz;\n"
	"}\n";

const char frag[] =
	"#version 150 core\n"
	"uniform sampler2D diffTex;\n"
	"in Vertex {\n"
	"	vec3 norm;\n"
	"	vec3 worldPos;\n"
	"	vec2 texCoord;\n"
	"} IN;\n"
	"out vec4 fragColour;\n"
	"void main() {\n"
	"	fragColour = vec4(sin(IN.worldPos.x), cos(IN.worldPos.y), sin(IN.worldPos.z), 1);\n"
	"}\n";

int main() {
	int swidth, sheight;
	Window win(swidth = 1280, sheight = 720);

	GLenum err = glewInit();
	if(err != GLEW_OK) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		return -1;
	}


	Shader shader(vert, frag);
	shader.use();
	DebugCamera dbcam(Vector3(10, 10, 10), 3.14f/4, -7*3.14f/32);

	int modelLoc = glGetUniformLocation(shader.getProgram(), "model");
	int viewLoc = glGetUniformLocation(shader.getProgram(), "view");
	int projLoc = glGetUniformLocation(shader.getProgram(), "proj");
	
	Matrix4 model = Matrix4::diagonal(10,10,10,1);
	Matrix4 proj = Matrix4::perspective(60, sheight*1.0f/swidth, 0.1f, 1000);
	Matrix4 view;
	
	shader.send(modelLoc, 1, &model);
	shader.send(projLoc, 1, &proj);
	
	VBOTestResource test;
	win.show();

	glEnable(GL_DEPTH_TEST);

	GameTime::startTime();

	int running = true;
	while (running) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		GameTime::startFrame();

		view = dbcam.getTransformMatrix();
		shader.send(viewLoc, 1, &view);

		dbcam.updateView();
		test.update();
		shader.use();
		test.draw();

		win.swapBuffers();
		win.update();
	}

	return 0;
}