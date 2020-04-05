#version 450

layout( location = 0 ) out vec4 frag_color;

layout (location = 0) in vec4 fragColor;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 fragPos;

void main() 
{  
	const vec3 lightDir = normalize(vec3(1,1,0.5));
	float diffuse = 0.6f+0.4f*max(dot(normal, lightDir),-dot(normal, lightDir));

	frag_color = vec4( diffuse * fragColor.xyz, fragColor.w );
}