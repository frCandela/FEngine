#version 450

layout( location = 0 ) out vec4 outColor;

layout( location = 0 ) in vec4 inColor;
layout( location = 1 ) in vec2 inTexCoord;

layout(set = 1, binding = 0) uniform sampler2D diffuseTexture;

void main() {  
	vec4 textureColor = texture(diffuseTexture, inTexCoord);

	outColor = textureColor *inColor;
}

