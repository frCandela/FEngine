#version 450

layout(binding = 0) uniform UniformBufferObject 
{
	vec2 screenSize;
} ubo;

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec4 inColor;

layout( location = 0 ) out vec4 outColor;

void main() 
{
    vec2 screenPos = 2.f * ( inPosition / ubo.screenSize ) - vec2(1.f,1.f );

    gl_Position = vec4( screenPos, 0, 1);
    outColor = inColor;
}