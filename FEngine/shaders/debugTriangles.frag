#version 450

layout( location = 0 ) out vec4 frag_color;

layout (location = 0) in vec4 fragColor;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 fragPos;

void main() {  
	vec3 lightPos = vec3(0,1000,0);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diffuse = 0.6+0.4*max(dot(normal, lightDir),-dot(normal, lightDir));

	frag_color = vec4( diffuse * fragColor.xyz, fragColor.w );
}