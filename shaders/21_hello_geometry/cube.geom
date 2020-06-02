#version 330 core
layout(points) in; // entrée de primitives
layout(triangle_strip, max_vertices = 65) out; //types sorties + nombres de primitives

//gl_in contain position, pointSize and clipDistance
//EmitVertex(); //generation of new vertex
//EndPrimitive(); // return new primitives

in float vSides[];

in vec3 vColor[]; // Sortie du vertex shader, pour chaque sommet

out vec3 fColor; // Sortie du fragment shader

const float PI = 3.1415926;

void main()
{

    for (int i = 0; i <= vSides[0]; i++) {
        // Engle entre chaque côté en radian
        float ang = PI * 2.0 / vSides[0] * i;

        // Décalage à partir du centre
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