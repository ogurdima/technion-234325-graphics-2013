#version 150 

#define MAX_LIGHTS 30

in vec4 vPosition;
in vec4 vNormal;
//in vec4 vColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalTransform;

uniform	vec4 emissive;
uniform vec4 diffuse;
uniform vec4 ambient;
uniform vec4 specular;
uniform float shininess;

uniform vec3 lightDir[MAX_LIGHTS];
uniform int lightNum;

out vec4 color;

void main()
{
	
	vec3 normalCf = normalize(view * normalTransform * vNormal).xyz;
	vec4 vertexCf = view * model * vPosition;
	vec3 diffuseColor = vec4(0,0,0,0);
	for (int i = 0; i < lightNum; i++)
	{
		diffuseColor += diffuse * max(0.5, dot(normalCf, normalize(lightDir[i])));
	}
	
	gl_Position = projection * vertexCf;
	color = vec4(1,0,0,1) * max(dot(normalCf, vec3(0,0,-1)), 0);
}
