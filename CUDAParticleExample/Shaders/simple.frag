#version 150 core

uniform sampler2D diffTex;

in Vertex {
	vec3 norm;
	vec3 worldPos;
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main() {
	fragColour = vec4(sin(IN.worldPos.x), cos(IN.worldPos.y), sin(IN.worldPos.z), 1);
}