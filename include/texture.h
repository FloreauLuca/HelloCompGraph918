#ifndef TEXTURE_H
#define TEXTURE_H

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Texture
{
public:
   
    void Init(const char* texturePath);
    

    void BindTexture();

    void Render();

    void Destroy();
	
private:
	unsigned texture;
    
};
#endif