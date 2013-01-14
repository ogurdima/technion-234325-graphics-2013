#version 150 

in vec4 vPosition;
//in vec4 vNormal;
//in vec4 vColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 color;

void main()
{
	gl_Position = projection * view * model * vPosition;
	color = vec4(1,0,0,0);
}
