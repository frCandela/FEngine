#version 450

layout(binding = 0) uniform UniformBufferObject 
{
    mat4 model;
    mat4 view;
    mat4 proj;
	vec4 color;
} ubo;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec4 inColor;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 debugColor;

void main() 
{

	vec4 worldVertex =  ubo.view * ubo.model * vec4(inPosition, 1.0);
	worldVertex.xyz = 0.99 * worldVertex.xyz; // slightly moves the vertex towards the camera to draw over geometry in wireframe mode

	gl_Position = ubo.proj * worldVertex;// ubo.view * ubo.model * vec4(inPosition, 1.0);
	fragColor = inColor;
	debugColor = ubo.color;
}