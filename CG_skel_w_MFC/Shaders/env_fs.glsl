// Fragment shader for ENV
#version 110 

in vec3 R; // in CF
varying out vec4 fcolor;

uniform samplerCube envCubeMap;

void main()
{
	vec4 texColor = textureCube(envCubeMap, R);
	fcolor = vec4(0.1, 0.1, 0, 1) + texColor * 0.9;
	//fcolor = vec4(R, 1);
}

