
#version 330 core

out vec4 FragColor;

in vec3 FragPos;  
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 color;

void main()
{   
  	
    vec3 result = color;
    FragColor = vec4(result, 1.0);
}
