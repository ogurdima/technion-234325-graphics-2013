// Vertex shader for PHONG shading
#version 150 

in vec4 vPosition;
in vec4 vNormal;
in vec2 vTex;

in vec3 vTan;
in vec3 vBitan;

out vec3 normal;
out vec4 vertex;
out vec2 fTex;
out vec3 fTan;
out vec3 fBitan;

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
	fTan = vTan;
	fBitan = vBitan;
}


