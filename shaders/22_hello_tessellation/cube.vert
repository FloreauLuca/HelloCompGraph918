
#version 330 core
layout(location = 0) in vec3 position;
out vec4 vposition;

out vec3 fColor;

void main()
{
    vposition = vec4(position, 1.0);
}