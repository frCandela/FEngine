#version 450       
	   
  
	   
layout (location = 0) in vec2 inUV;

layout( location = 0 ) out vec4 outColor; 
      
layout( set = 0, binding = 0 ) uniform sampler2D forwardSampler;    
layout( set = 1, binding = 0) uniform Uniforms
{
	vec4 color; 
} uniforms;

void main() {   
	outColor = uniforms.color * texture(forwardSampler, inUV);
}