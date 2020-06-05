#version 430

uniform mat4 mvp;
layout(quads) in;
in vec4 tposition[];

void main()
{
   float x = gl_TessCoord[0] + tposition[0].x;
   float y = gl_TessCoord[1] + tposition[0].z;
   float z = 0.0;

   gl_Position = vec4(x, y, z, 1.0);

}