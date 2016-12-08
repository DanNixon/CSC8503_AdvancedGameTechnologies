#version 150 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out Vertex {
	vec3 norm;
	vec3 worldPos;
	vec2 texCoord;
} OUT;

void main() {

	mat4 mv = view*model;
	vec4 worldPos = model*vec4(position, 1);
	gl_Position = proj*view*worldPos;
	OUT.norm = normalize(mat3(model)*normal);
	OUT.texCoord = texCoord;
	OUT.worldPos = worldPos.xyz;
}