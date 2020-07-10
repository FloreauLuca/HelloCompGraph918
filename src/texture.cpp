
#include "texture.h"
#include <GL/glew.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void Texture::Init(const char* texturePath)
{
	glGenTextures(1, &texture); //num texture, ID
	glBindTexture(GL_TEXTURE_2D, texture); //type data
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	unsigned char* data = stbi_load(texturePath, &width, &height,
		&nrChannels, 0);  
	if (data)
	{
		GLenum format = GL_RGB;
		switch(nrChannels)
		{
		case 1:
			format = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default :
			format = GL_RGB;
			break;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE,
			data);
		//type, level, internalFormat, width, height, border, format, type, data
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

}


void Texture::BindTexture()
{
	glBindTexture(GL_TEXTURE_2D, texture);
}

void Texture::Render()
{
}

void Texture::Destroy()
{
	glDeleteTextures(1, &texture);
}