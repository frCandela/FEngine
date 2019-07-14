#version 450

layout( location = 0 ) out vec4 outColor;

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec3 inFragPos;
layout (location = 2) in vec3 inNormal;

void main() {  
	// External data
	const vec3 lightPos = vec3(0.f,2.f,0.f);
	const vec3 diffuseColor = vec3(1,1,1);
	const float ambiant = 0.2f; 
	const float diffusePower = 1.f -  ambiant;


	// Diffuse light
	vec3 lightDir = lightPos - inFragPos;
	const float distance = length( lightDir );
	lightDir /= distance;
	const float NdotL = dot( inNormal, lightDir );
	const float intensity = clamp(NdotL,0.f,1.f);
	const float diffuse = intensity * diffusePower / distance;


	vec3 color = (ambiant + diffuse) * diffuseColor;
	outColor = vec4(color, 1.f );
}