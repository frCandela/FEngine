#version 450

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec4 inColor;

layout (binding = 0) uniform DynamicUniformBufferObject {
	vec2 position;
	vec2 scale;
} transform;

void main() {

	vec2 transformdPos = transform.scale * inPosition + transform.position;

	gl_Position = vec4( transformdPos, 0, 1);
}