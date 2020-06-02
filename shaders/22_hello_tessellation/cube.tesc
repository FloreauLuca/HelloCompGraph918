
#version 430

layout(vertices = 4) out;
in vec4 vposition[];
out vec4 tposition[];

void main()
{
   tposition[gl_InvocationID] = vposition[gl_InvocationID];

   if (gl_InvocationID == 0)
   {
       float tessLevel = 5.0;

       gl_TessLevelInner[0] = tessLevel;
       gl_TessLevelInner[1] = tessLevel;

       gl_TessLevelOuter[0] = tessLevel;
       gl_TessLevelOuter[1] = tessLevel;
       gl_TessLevelOuter[2] = tessLevel;
       gl_TessLevelOuter[3] = tessLevel;
   }
}