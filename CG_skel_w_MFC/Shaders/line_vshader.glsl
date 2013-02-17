#version 150

in vec4 vPosition;
in vec3 vColor;

uniform mat4 view;
uniform mat4 projection;

out vec3 color;
 
void main()
{
    color = vColor;
    gl_Position = projection * view * vPosition ;
}
