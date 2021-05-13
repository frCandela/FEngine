#version 450       
	     
layout (location = 0) in vec2 inUV;

layout( location = 0 ) out vec4 outColor; 
      
layout( set = 0, binding = 0 ) uniform sampler2D ppSampler;    
layout( set = 1, binding = 0) uniform Uniforms
{
	vec4 color; 
} uniforms;

void main()
{   
	outColor = uniforms.color * texture(ppSampler, inUV);
	//outColor = vec4(1.f, 0.f ,0.f, 1.f );
	outColor.a = 1.f;
}