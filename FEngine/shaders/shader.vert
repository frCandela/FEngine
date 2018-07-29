#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (binding = 0) uniform UboView 
{
	mat4 projection;
	mat4 view;
} uboView;

layout (binding = 1) uniform UboInstance 
{
	mat4 model; 
} uboInstance;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inTexCoord;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec2 fragTexCoord;

out gl_PerVertex 
{
	vec4 gl_Position;   
};

void main() 
{	
	mat4 modelView = uboView.view * uboInstance.model;
	gl_Position = uboView.projection * modelView * vec4(inPos.xyz, 1.0);

	fragColor = inColor;
	fragTexCoord = inTexCoord;
}
