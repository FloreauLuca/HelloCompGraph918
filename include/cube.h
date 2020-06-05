#ifndef RENDER_H
#define RENDER_H

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class RenderCube
{
public:
   
    RenderCube();

    void Init();

    void BindVao();

    void Render();

    void Destroy();
	
private:
    unsigned vao_ = 0;
    unsigned vbo_ = 0;
    
};

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class RenderQuad
{
public:

    RenderQuad();

    void Init();

    void BindVao();

    void Render();

    void Destroy();

private:
    unsigned vao_ = 0;
    unsigned vbo_ = 0;

};

class RenderCubeMaps
{
public:

    RenderCubeMaps();

    void Init();

    void BindVao();

    void Render();

    void Destroy();

private:
    unsigned vao_ = 0;
    unsigned vbo_ = 0;

};
#endif