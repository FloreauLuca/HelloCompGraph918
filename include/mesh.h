#ifndef MESH_H
#define MESH_H


#include <glm/vec3.hpp>
#include <string>
#include <vector>

#include "shader.h"

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

struct TextureMesh
{
	unsigned int id;
	std::string type;
	std::string path;  // we store the path of the texture to compare with other textures
};

class Mesh
{
public:
	// mesh data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<TextureMesh> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
	     std::vector<TextureMesh> textures);
	void Draw(Shader shader);
	//  render data
	unsigned int VAO, VBO, EBO;
private:

	void setupMesh();
};
#endif
