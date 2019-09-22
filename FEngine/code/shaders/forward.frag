#version 450
#extension GL_EXT_nonuniform_qualifier : require

layout( location = 0 ) out vec4 outColor;

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec3 inFragPos;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inTexCoord;

layout(binding = 2) uniform FragUniforms {
	vec3	cameraPosition;
	int		specularHardness;
	float	ambiantIntensity;		
} uniforms;

layout (binding = 3) uniform DynamicUniformBufferFrag {
	int	textureIndex;
} dynamicUbo;

struct PointLight {
	vec3	position;
	float	constant;
	vec3	diffuse;
	float	linear;
	vec3	specular;
	float	quadratic;
	vec3	ambiant;
};

layout(binding = 4) uniform LightUniform {
	PointLight pointLights[16];
	int lightNum;
} uniformLight;

layout(set = 1, binding = 0) uniform sampler2D texSampler[];

//https://learnopengl.com/Lighting/Multiple-lights
vec3 CalcPointLight(const PointLight light, const vec3 normal, const vec3 fragPos,  vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uniforms.specularHardness);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambiant  * vec3(texture(texSampler[dynamicUbo.textureIndex], inTexCoord));
    vec3 diffuse  = light.diffuse  * vec3(diff) * vec3(texture(texSampler[dynamicUbo.textureIndex], inTexCoord));
    vec3 specular = light.specular * vec3(spec);// * vec3(texture(material.specular, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 

void main() {  
	//Needed data
	const float diffusePower = 1.f -  uniforms.ambiantIntensity;
	vec3 goodNormal = normalize(inNormal);
	vec3 lightDir = uniformLight.pointLights[uniformLight.lightNum].position - inFragPos;
	const float distance = length( lightDir );
	lightDir /= distance;
	const vec3 viewDir = normalize(uniforms.cameraPosition - inFragPos);
	const vec4 textureColor = texture(texSampler[dynamicUbo.textureIndex], inTexCoord);

	vec3 lightColor = vec3(0,0,0);
	for(int lightIndex = 0; lightIndex < uniformLight.lightNum; lightIndex++) {
		lightColor += CalcPointLight( uniformLight.pointLights[lightIndex], goodNormal, inFragPos, viewDir );
	}

	outColor = vec4(lightColor,1);
}










































