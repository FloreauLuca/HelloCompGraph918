#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"
#include "shader.h"
/* assimp include files. These three are usually needed. */
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{
public:
    void loadModel(std::string path);
    void Draw(Shader shader);
    // model data
    std::vector<Mesh> meshes;
private:
    std::vector<TextureMesh> textures_loaded;
    std::string directory;

    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    unsigned int TextureFromFile(const char* path, const std::string& directory);
    std::vector<TextureMesh> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
                                              std::string typeName);
};
#endif
