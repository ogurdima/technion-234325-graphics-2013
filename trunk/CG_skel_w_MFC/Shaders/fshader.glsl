#version 130
#extension GL_ARB_texture_rectangle : enable
in  vec2 texCoord;

out vec4 fColor;

uniform sampler2DRect texture;

void main() 
{ 
   fColor = texture2DRect( texture, texCoord);
   //fColor = vec4(0,0,1,1);
} 

