#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 2) uniform sampler2D texSampler;

layout (location = 0) in vec3 inFragColor;
layout (location = 1) in vec2 inFragTexCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inToLight;

layout (location = 0) out vec4 outColor;

void main() 
{		
	float ambiantTerm = 0.1;
	float diffuseTerm = clamp(dot(inNormal, inToLight), 0, 1) ;

	outColor = (ambiantTerm + diffuseTerm) * vec4(inFragColor, 1.0) * texture(texSampler, inFragTexCoord);
}