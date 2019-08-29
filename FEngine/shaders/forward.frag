#version 450

layout( location = 0 ) out vec4 outColor;

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec3 inFragPos;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inTexCoord;

layout(binding = 2) uniform FragUniforms {

	vec3 cameraPosition;
	float ambiantIntensity;
	vec3 lightColor;
	int specularHardness;
	vec3 lightPos;
} uniforms;

//layout(binding = 3) uniform sampler2D texSampler;
layout(set = 1, binding = 0) uniform sampler2D texSampler[];

void main() {  

	//Needed data
	const float diffusePower = 1.f -  uniforms.ambiantIntensity;
	vec3 goodNormal = normalize(inNormal);
	vec3 lightDir = uniforms.lightPos - inFragPos;
	const float distance = length( lightDir );
	lightDir /= distance;
	const vec3 viewDir = normalize(uniforms.cameraPosition - inFragPos);
	const vec4 textureColor = texture(texSampler[0], inTexCoord);

	// Diffuse light
	const float NdotL = dot( goodNormal, lightDir );
	const float diffuseIntensity = clamp(NdotL,0.f,1.f) * diffusePower / (0.1f*distance);

	// Specular
	vec3 H = normalize( lightDir + viewDir );
	float NdotH = dot( goodNormal, H );
	float specularIntensity = pow( clamp(NdotH ,0.f,1.f), uniforms.specularHardness );

	float totalIntensity = specularIntensity + uniforms.ambiantIntensity + diffuseIntensity;
	outColor = vec4( totalIntensity * uniforms.lightColor, 1.f );	
	
	outColor = vec4( textureColor.xyz * totalIntensity * vec3(1,1,1), 1.f );
}