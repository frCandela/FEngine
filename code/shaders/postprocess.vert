#version 450  

layout( location = 0 ) in vec4 inPosition;       

layout (location = 0) out vec2 outUV;

void main() {         
	outUV = vec2((inPosition.x + 1) / 2,(inPosition.y + 1) / 2);

	gl_Position = inPosition;       
}