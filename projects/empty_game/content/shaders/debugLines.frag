#version 450

layout( location = 0 ) out vec4 frag_color;

layout (location = 0) in vec4 fragColor;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 fragPos;
layout (location = 3) in vec4 debugColor;

void main() 
{  
	frag_color = debugColor * fragColor;
}