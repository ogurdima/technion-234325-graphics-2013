// Fragment shader for TOON shading
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
float quant(in float val);

in vec3 normal;
in vec4 vertex;
in vec2 fTex;

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
	vec3 viewDir = normalize(-vertex.xyz);

	MaterialColor mc;
	mc.diffuse = diffuse;
	mc.specular = vec3(0,0,0); // no specular in TOON
	mc.emissive = emissive; 
	mc.ambient = ambient;
	mc.shininess = shininess;

	if ( useTex )
	{
		mc.diffuse = texture2D(texMap, fTex).rgb;
	}

	vec3 totalColor = calcColor(vertex.xyz, normal, viewDir, mc);
	vec3 tooned = vec3(quant(totalColor.r), quant(totalColor.g), quant(totalColor.b));
	fcolor = vec4(tooned, 1);
}


vec3 calcColor(in vec3 V, in vec3 N, in vec3 viewDir, in MaterialColor mc) 
{
	vec3 df = vec3(0,0,0);
	vec3 af = vec3(0,0,0);
	vec3 sf = vec3(0,0,0);
	vec3 ef = mc.emissive;
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

float quant(in float val)
{
	if (val > 0.95)
		val = 0.95;
	else if (val > 0.7)
		val = 0.7;
	else if (val > 0.4)
		val = 0.4;
	else if (val > 0.3)
		val = 0.3;
	else if (val > 0.2)
		val = 0.2;
	else if (val > 0.1)
		val = 0.1;
	else
		val = 0;
	return val;
}
