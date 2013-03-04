// Vertex shader for FLAT and GOURAUD rendering
#version 150 

#define MAX_LIGHTS 30

struct MaterialColor {
	vec3	emissive;
	vec3	diffuse;
	vec3	ambient;
	vec3	specular;
	float	shininess;
};

vec3 calcColor(in vec3 V, in vec3 N, in vec3 viewDir, in MaterialColor mc);

in vec4 vPosition;
in vec4 vNormal;
out vec4 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalTransform;

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

void main()
{
	vec3 normalCf = normalize(view * normalTransform * vNormal).xyz;
	vec4 vertexCf = view * model * vPosition;
	vec3 viewDir = normalize(-vertexCf.xyz);

	MaterialColor mc;
	mc.diffuse = diffuse;
	mc.specular = specular;
	mc.emissive = emissive;
	mc.ambient = ambient;
	mc.shininess = shininess;

	gl_Position = projection * vertexCf;
	vec3 totalColor = calcColor(vertexCf.xyz, normalCf, viewDir, mc);
	color = vec4(totalColor, 1);
}

vec3 calcColor(in vec3 V, in vec3 N, in vec3 viewDir, in MaterialColor mc) 
{
	vec3 df = vec3(0,0,0);
	vec3 af = vec3(0,0,0);
	vec3 sf = vec3(0,0,0);
	vec3 ef = mc.emissive;
	N = normalize(N);
	for (int i = 0; i < parallelLightNum; i++)
	{
		vec3 L = normalize(lightDir[i]);
		float NdotL =  max(0.0, dot(N, -L ) );
		df += mc.diffuse * NdotL * parlightColor[i];
		if (NdotL > 0)
		{
			sf += mc.specular * parlightColor[i] * pow( max(   dot(reflect(L, N), viewDir),    0) , mc.shininess);
		}
	}
	for (int i = 0; i < pointLightNum; i++)
	{
		vec3 L = normalize(V - lightPos[i]);
		float NdotL =  max(0.0, dot(N, -L ) );
		df += mc.diffuse * NdotL * ptlightColor[i];
		if (NdotL > 0)
		{
			sf += mc.specular * ptlightColor[i] * pow( max(   dot(reflect(L, N), viewDir),    0) , mc.shininess);
		}
	}
	vec3 totalColor = df + af + sf + ef;
	return totalColor;
}

