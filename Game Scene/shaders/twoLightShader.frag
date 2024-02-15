#version 410 core

in vec4 fPosEye;
in vec3 fNormal;
in vec2 fTexCoords;

in float fAlpha;

out vec4 fColor;


//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

//lighting
uniform vec3 lightPosEye[2];
uniform vec3 lightDir[2];
uniform vec3 lightColor[2];
uniform int activeLight[2];


// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

vec3 ambient[2];
float ambientStrength = 0.2f;
vec3 diffuse[2];
vec3 specular[2];
float specularStrength = 0.5f;
float shininess = 32.0f;


float constant = 1.0f;
float linear = 0.004f;
float quadratic = 0.0001f;

void computeLightComponents(int index)
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightPosEye[index] - fPosEye.xyz);

	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
	ambient[index] = ambientStrength * lightColor[index];
	
	//compute diffuse light
	diffuse[index] = max(dot(normalEye, lightDirN), 0.0f) * lightColor[index];
	

	//compute half vector
	vec3 halfVector = normalize(lightDirN + viewDirN);

	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);

	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);


	//compute distance to light
	float dist = length(lightPosEye[index] - fPosEye.xyz);
	//compute attenuation
	float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));


	if(activeLight[index] == 1){

	ambient[index] = att * ambientStrength * lightColor[index];
	diffuse[index] = att * max(dot(normalEye, lightDirN), 0.0f) * lightColor[index];
	specular[index] = att * specularStrength * specCoeff * lightColor[index];

	}
	else{

	    ambient[index] = vec3(0.0f);
        diffuse[index] = vec3(0.0f);
        specular[index] = vec3(0.0f);
	
	}



}


void main() 
{


	//computeLightComponents(0);
	//vec3 color = min((ambient[0] + diffuse[0]) * texture(diffuseTexture, fTexCoords).rgb + specular[0] * texture(specularTexture, fTexCoords).rgb, 1.0f);
    //fColor = vec4(color, 1.0f);

	computeLightComponents(0);
	computeLightComponents(1);


    vec3 color = min((ambient[0] + diffuse[0]) * texture(diffuseTexture, fTexCoords).rgb +
                         specular[0] * texture(specularTexture, fTexCoords).rgb +
                         (ambient[1] + diffuse[1]) * texture(diffuseTexture, fTexCoords).rgb +
                         specular[1] * texture(specularTexture, fTexCoords).rgb,
                     1.0f);

    fColor = vec4(color, fAlpha);


}
