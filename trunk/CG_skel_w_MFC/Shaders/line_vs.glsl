// Vertex shader for lines
#version 150

in vec4 vPosition;
in vec3 vColor;
out vec4 color;

uniform mat4 view;
uniform mat4 projection;
 
void main()
{
    color = vec4(vColor, 1);
    gl_Position = projection * view * vPosition;
}
