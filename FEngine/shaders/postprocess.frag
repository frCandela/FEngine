       #version 450       
	   
	   layout( set = 0, binding = 0 ) uniform sampler2D forwardSampler;     
	   
	   layout (location = 0) in vec2 inUV;

	   layout( location = 0 ) out vec4 outColor;       
	   
	   void main() {   
			outColor = vec4(0.2,0.4,0.6,1) *texture(forwardSampler, inUV);
		}