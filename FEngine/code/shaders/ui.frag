#version 450

layout( location = 0 ) out vec4 outColor;

layout( location = 0 ) in vec4 inColor;
layout( location = 1 ) in vec2 inTexCoord;

void main() {  
	outColor = vec4(inTexCoord.xy, 0,1.f);

}

