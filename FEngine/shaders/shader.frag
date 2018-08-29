#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 2) uniform sampler2D texSampler;

layout (location = 0) in vec3 inFragColor;
layout (location = 1) in vec2 inFragTexCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inLightPos;
layout (location = 4) in float inAmbiant;
layout (location = 5) in vec3 inViewPos;
layout (location = 6) in vec3 inFragPos;
layout (location = 7) in float inSpecularExponent;

layout (location = 0) out vec4 outColor;

void main() 
{			
	vec3 toLight = normalize(inLightPos - inFragPos.xyz);
	vec3 viewDir = normalize(inViewPos - inFragPos);
    vec3 halfwayDir = normalize(toLight + viewDir);

    float specular = pow(max(dot(inNormal, halfwayDir), 0.0), inSpecularExponent);
	float diffuse = clamp(dot(inNormal, toLight), 0, 1) ;

	// wrongOrientation is 1 or zero depending on the diffuse value.
	float wrongOrientation = clamp(10000*diffuse, 0, 1) ;

	outColor = (inAmbiant + diffuse + wrongOrientation*specular) * vec4(inFragColor, 1.0) * texture(texSampler, inFragTexCoord);
}