#version 450

layout( location = 0 ) out vec4 outColor;

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec3 inFragPos;
layout (location = 2) in vec3 inNormal;

layout(binding = 2) uniform FragUniforms {

	vec3 cameraPosition;
	float ambiantIntensity;
	vec3 lightColor;
	int specularHardness;
} uniforms;

void main() {  
	// External data
	const vec3 lightPos = vec3(0.f,2.f,0.f);

	//Needed data
	const float diffusePower = 1.f -  uniforms.ambiantIntensity;
	vec3 goodNormal = normalize(inNormal);
	vec3 lightDir = lightPos - inFragPos;
	const float distance = length( lightDir );
	lightDir /= distance;
	const vec3 viewDir = normalize(uniforms.cameraPosition - inFragPos);

	// Diffuse light
	const float NdotL = dot( goodNormal, lightDir );
	const float diffuseIntensity = clamp(NdotL,0.f,1.f) * diffusePower / (0.1f*distance);

	// Specular
	vec3 H = normalize( lightDir + viewDir );
	float NdotH = dot( goodNormal, H );
	float specularIntensity = pow( clamp(NdotH ,0.f,1.f), uniforms.specularHardness );

	float totalIntensity = specularIntensity + uniforms.ambiantIntensity + diffuseIntensity;
	outColor = vec4( totalIntensity * uniforms.lightColor, 1.f );
	
	//outColor = vec4( totalIntensity * vec3(1,1,1), 1.f );

}