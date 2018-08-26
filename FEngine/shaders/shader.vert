#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 0) uniform UboView 
{
	mat4 projection;
	mat4 view;
	vec3 viewPosition;
	float ambiant;
} uboView;

layout (binding = 1) uniform UboInstance 
{
	mat4 model;
} uboInstance;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inNormal;

layout (location = 0) out vec3 outFragColor;
layout (location = 1) out vec2 outFragTexCoord;
layout (location = 2) out vec3 outNormal;
layout (location = 3) out vec3 outLightPos;
layout (location = 4) out float outAmbiant;
layout (location = 5) out vec3 outViewPos;
layout (location = 6) out vec3 outFragPos;

out gl_PerVertex 
{
	vec4 gl_Position;   
};

void main() 
{	
	vec4 worldPos = uboInstance.model * vec4(inPos, 1.0);
	vec3 lightPos = vec3(0,0,0);

	gl_Position = uboView.projection * uboView.view * worldPos;

	outFragColor = inColor;
	outFragTexCoord = inTexCoord;
	outNormal = inNormal;
	outLightPos = lightPos;
	outAmbiant = uboView.ambiant;
	outViewPos = uboView.viewPosition;
	outFragPos = worldPos.xyz;
}
