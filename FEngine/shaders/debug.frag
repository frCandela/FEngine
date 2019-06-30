#version 450

layout( location = 0 ) out vec4 frag_color;

layout(location = 0) in vec3 fragColor;

void main() {  
	frag_color = vec4( fragColor, 1.0 );
}