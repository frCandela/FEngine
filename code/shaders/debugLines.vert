#version 450

layout(binding = 0) uniform UniformBufferObject 
{
    mat4 model;
    mat4 view;
    mat4 proj;
	vec4 color;
} ubo;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec4 inColor;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec3 fragPos;
layout (location = 3) out vec4 debugColor;

void main() {
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
	fragColor = inColor;
	normal = inNormal;
	fragPos = vec3(ubo.model * vec4(inPosition, 1.0));
	debugColor = ubo.color;
}