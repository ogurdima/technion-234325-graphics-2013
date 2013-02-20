// Fragment shader for FLAT and GOURAUD rendering
#version 150 

in vec4 color;
out vec4 fcolor;

void main()
{
	fcolor = color;
}
