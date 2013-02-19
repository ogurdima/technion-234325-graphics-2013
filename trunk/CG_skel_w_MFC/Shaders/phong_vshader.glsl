#version 150 

in vec4 vPosition;
in vec4 vNormal;
in vec2 vTex;

out vec3 normal;
out vec4 vertex;
smooth out vec2 fTex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalTransform;


void main()
{
	vec3 normalCf = normalize(view * normalTransform * vNormal).xyz;
	vec4 vertexCf = view * model * vPosition;

	gl_Position = projection * vertexCf;
	normal = normalCf;
	vertex = vertexCf;
	fTex = vTex;
}


