#version 150 

in vec4 vPosition;
in vec4 vNormal;
out vec4 vCol;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalTransform;


void main()
{
	vec3 normalCf = normalize(view * normalTransform * vNormal).xyz;
	vec4 vertexCf = view * model * vPosition;
	
	vertexCf += vec4( normalCf, 0) * 0.1;

	gl_Position = projection * vertexCf;
	vCol = vec4(0,0,0,1);
}


