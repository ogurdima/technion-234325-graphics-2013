// Vertex shader for PHONG shading
#version 150 

in vec4 vPosition;
in vec4 vNormal;

out vec3 R;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalTransform;

void main()
{
	vec3 normalCf = normalize(view * normalTransform * vNormal).xyz;
	vec4 vertexCf = view * model * vPosition;

	gl_Position = projection * vertexCf;

	vec3 eyePos = vertexCf.xyz;
	vec3 N = normalCf;
	R = reflect(eyePos, N);
}


