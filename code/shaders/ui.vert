#version 450

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec4 inColor;

layout( location = 0 ) out vec4 outColor;
layout (location = 1) out vec2  outTexCoord;

layout (binding = 0) uniform DynamicUniformBufferObject 
{
	vec2 position;
	vec2 scale;
	vec4 color;
} transform;

void main()
{

	vec2 transformdPos = transform.scale * inPosition + transform.position;

	gl_Position = vec4( transformdPos, 0, 1);

	outColor = transform.color;
	outTexCoord = inUV;
}