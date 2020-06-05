#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 65) out; 

in float vSides[];

in vec3 vColor[]; 

out vec3 fColor; 

const float PI = 3.1415926;

void main()
{

    for (int i = 0; i <= vSides[0]; i++) {
        float ang = PI * 2.0 / vSides[0] * i;

        vec4 offset = vec4(cos(ang) * 0.3, -sin(ang) * 0.4, 0.0, 0.0);
        gl_Position = gl_in[0].gl_Position + offset;
        fColor = vColor[0] + i/vSides[0];
        EmitVertex();
        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
    }

    EndPrimitive();
}

//void main()
//{
//    fColor = vColor[0];
//    
//    // Décalage à partir du centre
//    gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.1, 0.1, 0.0);
//    EmitVertex();
//    gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.1, 0.1, 0.0);
//    EmitVertex();
//    gl_Position = gl_in[0].gl_Position + vec4(-0.1, -0.1, 0.1, 0.0);
//    EmitVertex();
//    gl_Position = gl_in[0].gl_Position + vec4(0.1, -0.1, 0.1, 0.0);
//    EmitVertex();
//    gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.1, -0.1, 0.0);
//    EmitVertex();
//    gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.1, -0.1, 0.0);
//    EmitVertex();
//    gl_Position = gl_in[0].gl_Position + vec4(-0.1, -0.1, -0.1, 0.0);
//    EmitVertex();
//    gl_Position = gl_in[0].gl_Position + vec4(0.1, -0.1, -0.1, 0.0);
//    EmitVertex();
//
//    EndPrimitive();
//}