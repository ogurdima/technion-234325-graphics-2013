// Vertex shader for PHONG shading
#version 150 

in vec4 vPosition;
in vec4 vNormal;
in vec4 avgNormal;
in vec2 vTex;
in vec3 vRand;

in vec3 vTan;
in vec3 vBitan;

out vec3 normal;
out vec4 vertex;
out vec2 fTex;
out mat3 tbn;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalTransform;

uniform bool useNormalMap;
uniform bool useVertexAnimation;
uniform vec3 vertexAnimationParam;

void main()
{	
	vec4 vertexCf = view * model * vPosition;
	vec3 normalCf = normalize(view * normalTransform * vNormal).xyz;
	vec3 tanCf = normalize( view * normalTransform * vec4(vTan,0)).xyz;
	vec3 bitanCf = normalize( view * normalTransform * vec4(vBitan,0) ).xyz;
	vec3 avgNormalCf = normalize(view * normalTransform * avgNormal).xyz;
	
	if( useVertexAnimation)
	{
		float ppp = dot(vertexAnimationParam, vRand) / 10;
		vertexCf += (vec4(avgNormalCf,0) * ppp);
	}

	vec4 posCf = projection * vertexCf;
	gl_Position = posCf;
	normal = normalCf;
	vertex = vertexCf;
	fTex = vTex;
	
	if( useNormalMap)
	{
		tanCf = normalize(tanCf - normalCf * dot(normalCf, tanCf));
		bitanCf = normalize(bitanCf - normalCf * dot(normalCf, bitanCf));
		tbn =  mat3( tanCf, bitanCf, normalCf);
	}
}


