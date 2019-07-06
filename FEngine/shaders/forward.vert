#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout (binding = 1) uniform DynamicUniformBufferObject {
	mat4 model;
} dynamicUbo;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inTexCoord;

layout (location = 0) out vec3 fragColor;

void main() {
	gl_Position = ubo.proj * ubo.view * dynamicUbo.model * vec4(inPosition, 1.0);
	//fragColor = inColor;
	fragColor = vec3(inPosition.x,inPosition.y,inPosition.z);
}