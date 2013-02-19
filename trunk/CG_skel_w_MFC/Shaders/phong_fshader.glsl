#version 150 

#define MAX_LIGHTS 30


in vec3 normal;
in vec4 vertex;
smooth in vec2 fTex;

out vec4 fcolor;

uniform	vec3 emissive;
uniform vec3 diffuse;
uniform vec3 ambient;
uniform vec3 specular;
uniform float shininess;

uniform vec3 lightDir[MAX_LIGHTS];
uniform vec3 parlightColor[MAX_LIGHTS];
uniform int parallelLightNum;

uniform vec3 lightPos[MAX_LIGHTS];
uniform vec3 ptlightColor[MAX_LIGHTS];
uniform int pointLightNum;

uniform bool useTex;
uniform sampler2D texMap;

void main()
{
	vec4 vertexCf = vertex;
	vec3 normalCf = normal;
	
	vec3 diffuseFactor = vec3(0,0,0);
	vec3 ambientFactor = vec3(0,0,0);
	vec3 specularFactor = vec3(0,0,0);
	vec3 emissiveFactor = emissive;
	float NdotL;
	vec3 veiwDir = normalize(-vertexCf.xyz);
	
	vec3 tColor = diffuse;
	if( useTex)
	{
		tColor = texture2D( texMap, fTex).xyz;
	}

	for (int i = 0; i < parallelLightNum; i++)
	{
		vec3 L = normalize(lightDir[i]);
		NdotL =  max(0.0, dot(normalCf, -L ) );
		diffuseFactor += tColor * NdotL * parlightColor[i];
		if (NdotL > 0)
		{
			specularFactor += specular * parlightColor[i] * pow( max(   dot(reflect(L, normalCf), veiwDir),    0) , shininess);
		}
	}

	for (int i = 0; i < pointLightNum; i++)
	{
		vec3 L = normalize(vertexCf.xyz - lightPos[i]);
		NdotL =  max(0.0, dot(normalCf, -L ) );
		diffuseFactor += tColor * NdotL * ptlightColor[i];
		if (NdotL > 0)
		{
			specularFactor += specular * ptlightColor[i] * pow( max(   dot(reflect(L, normalCf), veiwDir),    0) , shininess);
		}
	}
	
	vec3 totalColor = diffuseFactor + ambientFactor + specularFactor + emissiveFactor;

	fcolor = vec4(totalColor, 1);
}
