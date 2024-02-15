#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;
in vec3 fPosition;

out vec4 fColor;

//lighting
uniform	vec3 lightDir;
uniform	vec3 lightColor;
uniform vec3 light2Dir;
uniform vec3 light2Color;

//texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

//fog
uniform float fogDensity;
uniform float valAtt;
uniform int pointLight;


vec3 ambient;
vec3 ambient2;
float ambientStrength = 0.2f;

vec3 diffuse;
vec3 diffuse2;

vec3 specular;
vec3 specular2;
float specularStrength = 0.5f;

float shininess = 32.0f;

float constant = 1.0f;
float linear = 0.0045f;
float quadratic = 0.0075f;

void computeDirLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	

	//compute distance to light
	float dist = length(cameraPosEye - fPosEye.xyz);
	//compute attenuation
	float att = valAtt / (constant + linear * dist + quadratic * (dist * dist));


	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
	ambient = att * ambientStrength * lightColor;

	//compute diffuse light
	diffuse = att * max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular = att * specularStrength * specCoeff * lightColor;



}


void computePointLightComponents(vec3 lDir){

	vec3 cameraPosEye = vec3(0.0f);

	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lDir);
	
	//compute view direction 
	//vec3 viewDirN = -normalize(cameraPosEye - fPosEye.xyz);
	
	//compute distance to light
	float dist = length(lDir - fPosition.xyz) * 0.2;
	
	if(dist < 0.5) {
		dist = -10;
		}
	//compute attenuation
	float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));
	
	//compute ambient light
	ambient2 = att * ambientStrength * light2Color;
	
	//compute diffuse light
	diffuse2 = att * max(dot(normalEye, lightDirN), 0.0f) * light2Color;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(lightDirN, reflection), 0.0f), shininess);
	specular2 = att * specularStrength * specCoeff * light2Color;

}


float computeShadow()
{
	// perform perspective divide
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	// Transform to [0,1] range
	normalizedCoords = normalizedCoords * 0.5 + 0.5;

	if (normalizedCoords.z > 1.0f)
		return 0.0f;

	// Get closest depth value from light's perspective
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;

	// Get depth of current fragment from light's perspective
	float currentDepth = normalizedCoords.z;

	// Check whether current frag pos is in shadow
	float bias = 0.005f;
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	return shadow;
	
}


float computeFog()
{
 float fragmentDistance = length(fPosEye);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
	computeDirLightComponents();
	if(pointLight == 1)
		computePointLightComponents(light2Dir);

	vec3 baseColor = vec3(0.9f, 0.35f, 0.0f);//orange
	
	ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;


	ambient2 *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse2 *= texture(diffuseTexture, fTexCoords).rgb;
	specular2 *= texture(specularTexture, fTexCoords).rgb;

	float shadow = computeShadow();

	//vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);
	vec3 color = min(((ambient+ambient2) + (1.0f - shadow)*(diffuse+diffuse2)) + (1.0f - shadow)*(specular+specular2), 1.0f);
	


    float fogFactor = computeFog();
    vec3 fogColor = vec3(0.5f, 0.5f, 0.5f);

	fColor = vec4( fogColor * (1 - fogFactor) + color * fogFactor, 1.0f);

}
