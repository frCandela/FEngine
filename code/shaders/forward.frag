#version 450

layout( location = 0 ) out vec4 outColor;

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec3 inFragPos;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inTexCoord;

layout(binding = 2) uniform FragUniforms {
	vec3	cameraPosition;	
} uniforms;

layout (binding = 3) uniform DynamicUniformBufferFrag {
	vec4 color;
	int shininess;
} material;

struct PointLight {
	vec4	position;
	vec4	diffuse;
	vec4	specular;
	vec4	ambiant;
	float	constant;
	float	linear;
	float	quadratic;
	float   _0;
};

struct DirectionalLight {
	vec4 direction; 
	vec4 ambiant;	
	vec4 diffuse;	
	vec4 specular;	
};

#define MAX_NUM_DIRECTIONNAL_LIGHTS 4  
#define MAX_NUM_POINT_LIGHTS 		16  
layout(binding = 4) uniform LightUniform {	
	DirectionalLight dirLights[MAX_NUM_DIRECTIONNAL_LIGHTS];
	PointLight pointLights[MAX_NUM_POINT_LIGHTS];
	int dirLightsNum;
	int pointLightNum;
} lights;

layout( set = 1, binding = 0 ) uniform texture2D diffuseTexture;     
layout( set = 2, binding = 0 ) uniform sampler   diffuseSampler;

vec3 CalcPointLight( const PointLight light, const vec3 normal, const vec3 fragPos,  vec3 viewDir);
vec3 CalcDirLight  ( const DirectionalLight light, const vec3 normal, const vec3 viewDir); 

//reference: https://learnopengl.com/Lighting/Multiple-lights
void main() {  
	//Needed data
	vec3 goodNormal = normalize(inNormal);
	const vec3 viewDir = normalize(uniforms.cameraPosition - inFragPos);

	vec3 lightColor = vec3(0,0,0);
	for(int lightIndex = 0; lightIndex < lights.dirLightsNum; lightIndex++) {
		lightColor += CalcDirLight( lights.dirLights[lightIndex], goodNormal, viewDir );
	}
	for(int lightIndex = 0; lightIndex < lights.pointLightNum; lightIndex++) {
		lightColor += CalcPointLight( lights.pointLights[lightIndex], goodNormal, inFragPos, viewDir );
	}

	float alpha = texture( sampler2D( diffuseTexture, diffuseSampler ), inTexCoord ).a * material.color.a;
	outColor = vec4(inColor * lightColor, alpha);
}


vec3 CalcDirLight  ( const DirectionalLight light, const vec3 normal, const vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction.xyz);
	
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
	
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
	vec3 textureColor = material.color.xyz * vec3(texture( sampler2D( diffuseTexture, diffuseSampler ), inTexCoord ));
    vec3 ambient  = light.ambiant.xyz  * textureColor;
    vec3 diffuse  = light.diffuse.xyz  * diff * textureColor;
    vec3 specular = light.specular.xyz * spec * vec3(texture( sampler2D( diffuseTexture, diffuseSampler ), inTexCoord ));
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(const PointLight light, const vec3 normal, const vec3 fragPos,  vec3 viewDir)
{
    vec3 lightDir = normalize(light.position.xyz - fragPos);
    
	// diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
	// specular shading	(blinn-phong)
	vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    
	// attenuation
    float distance    = length(light.position.xyz - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
	// combine results
	vec3 textureColor = material.color.xyz * vec3(texture( sampler2D( diffuseTexture, diffuseSampler ), inTexCoord ));
    vec3 ambient  = light.ambiant.xyz  * textureColor;
    vec3 diffuse  = light.diffuse.xyz  * vec3(diff) * textureColor;
    vec3 specular = light.specular.xyz * vec3(spec) * vec3(texture( sampler2D( diffuseTexture, diffuseSampler ), inTexCoord ));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 








































