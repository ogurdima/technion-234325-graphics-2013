// Vertex shader for silhouette rendering
#version 150 

in vec4 vPosition;
in vec4 vNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalTransform;

out vec4 color;

void main()
{
	vec3 normalCf = normalize(view * normalTransform * vNormal).xyz;
	vec4 vertexCf = view * model * vPosition;
	vertexCf += vec4(normalCf, 0) * 0.01;
	gl_Position = projection * vertexCf;
	color = vec4(0, 0, 0, 1);
}


