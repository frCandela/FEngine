#version 450

layout( location = 0 ) out vec4 outColor;

layout( location = 0 ) in vec4 inColor;
layout( location = 1 ) in vec2 inTexCoord;

layout( set = 1, binding = 0 ) uniform texture2D diffuseTexture;     
layout( set = 2, binding = 0 ) uniform sampler   diffuseSampler;


void main() {  
	vec4 textureColor = texture( sampler2D( diffuseTexture, diffuseSampler ), inTexCoord );
	outColor = textureColor *inColor;
}