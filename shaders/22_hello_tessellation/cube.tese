#version 430

uniform mat4 mvp;
layout(quads) in;
in vec4 tposition[];

void main()
{
   float x = gl_TessCoord[0] * (tposition[1].x - tposition[0].x) + tposition[0].x;
   float z = gl_TessCoord[1] * (tposition[1].z - tposition[2].z) + tposition[2].z;
   float y = 0.0;

   gl_Position = mvp * vec4(x, y, z, 1.0);

}