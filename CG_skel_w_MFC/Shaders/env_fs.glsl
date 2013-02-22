#version 150 
// Fragment shader for ENV


in vec3 R; // in CF
out vec4 fcolor;

uniform samplerCube envCubeMap;

void main()
{
	vec4 texColor = texture(envCubeMap, R);
	fcolor = vec4(0.8, 0.8, 0.8, 1) * 0.3 + texColor * 0.7;
	//fcolor = vec4(R, 1);
}

