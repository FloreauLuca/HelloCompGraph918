//
// Created by unite on 12/05/2020.
//

#include <fstream>

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#include <gli/gli.hpp>
#include <gli/generate_mipmaps.hpp>

bool ConvertToDds(char const* FilenameSrc, char const* FilenameDst)
{
    if (FilenameDst == nullptr)
        return false;
    if (std::strstr(FilenameDst, ".dds") > 0 || std::strstr(FilenameDst, ".ktx") > 0)
        return false;
    gli::texture2d TextureSource(gli::load(FilenameSrc));
    if (TextureSource.empty())
        return false;
    if (TextureSource.format() != gli::FORMAT_RGB16_SFLOAT_PACK16 && TextureSource.format() != gli::FORMAT_RGB32_SFLOAT_PACK32)
        return false;
    gli::texture2d TextureMipmaped = gli::generate_mipmaps(TextureSource, gli::FILTER_LINEAR);
    gli::texture2d TextureConverted = gli::convert(TextureMipmaped, gli::FORMAT_RGB9E5_UFLOAT_PACK32);
    gli::save(TextureConverted, FilenameDst);
    return true;
}

GLuint CreateTexture(char const* Filename)
{
    gli::texture texture = gli::load_dds(Filename);
    if (texture.empty())
        return 0;

    gli::gl gl(gli::gl::PROFILE_GL33);
    gli::gl::format const format = gl.translate(texture.format(), texture.swizzles());
    GLenum target = gl.translate(texture.target());

    GLuint textureName = 0;
    glGenTextures(1, &textureName);
    glBindTexture(target, textureName);
    glm::tvec3<GLsizei> const extent(texture.extent());
    GLsizei const faceTotal = static_cast<GLsizei>(texture.layers() * texture.faces());

    GLsizei width = extent.x;
    GLsizei height = (texture.target() == gli::TARGET_2D ? extent.y : faceTotal);
    switch (texture.target())
    {
    case gli::TARGET_1D:
        glTexStorage1D(
            target, static_cast<GLint>(texture.levels()), format.Internal, extent.x);
        break;
    case gli::TARGET_1D_ARRAY:
    case gli::TARGET_2D:
    case gli::TARGET_CUBE:
        for (size_t i = 0; i < texture.levels(); i++) {
            glTexImage2D(target, i, format.Internal, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            width = static_cast<GLsizei>(width / 2.f);
            height = static_cast<GLsizei>(height / 2.f);
        }

    default:
        assert(0);
        break;
    }
    for (std::size_t layer = 0; layer < texture.layers(); ++layer)
        for (std::size_t face = 0; face < texture.faces(); ++face)
            for (std::size_t level = 0; level < texture.levels(); ++level)
            {
                GLsizei const layerGl = static_cast<GLsizei>(layer);
                glm::tvec3<GLsizei> extent(texture.extent(level));
                target = gli::is_target_cube(texture.target())
                    ? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face)
                    : target;

                switch (texture.target())
                {
                case gli::TARGET_1D:
                    if (gli::is_compressed(texture.format()))
                        glCompressedTexSubImage1D(
                            target, static_cast<GLint>(level), 0, extent.x,
                            format.Internal, static_cast<GLsizei>(texture.size(level)),
                            texture.data(layer, face, level));
                    else
                        glTexSubImage1D(
                            target, static_cast<GLint>(level), 0, extent.x,
                            format.External, format.Type,
                            texture.data(layer, face, level));
                    break;
                case gli::TARGET_1D_ARRAY:
                case gli::TARGET_2D:
                case gli::TARGET_CUBE:
                    if (gli::is_compressed(texture.format()))
                        glCompressedTexSubImage2D(
                            target, static_cast<GLint>(level),
                            0, 0,
                            extent.x,
                            texture.target() == gli::TARGET_1D_ARRAY ? layerGl : extent.y,
                            format.Internal, static_cast<GLsizei>(texture.size(level)),
                            texture.data(layer, face, level));
                    else
                        glTexSubImage2D(
                            target, static_cast<GLint>(level),
                            0, 0,
                            extent.x,
                            texture.target() == gli::TARGET_1D_ARRAY ? layerGl : extent.y,
                            format.External, format.Type,
                            texture.data(layer, face, level));
                    break;
                case gli::TARGET_2D_ARRAY:
                case gli::TARGET_3D:
                case gli::TARGET_CUBE_ARRAY:
                    if (gli::is_compressed(texture.format()))
                        glCompressedTexSubImage3D(
                            target, static_cast<GLint>(level),
                            0, 0, 0,
                            extent.x, extent.y,
                            texture.target() == gli::TARGET_3D ? extent.z : layerGl,
                            format.Internal, static_cast<GLsizei>(texture.size(level)),
                            texture.data(layer, face, level));
                    else
                        glTexSubImage3D(
                            target, static_cast<GLint>(level),
                            0, 0, 0,
                            extent.x, extent.y,
                            texture.target() == gli::TARGET_3D ? extent.z : layerGl,
                            format.External, format.Type,
                            texture.data(layer, face, level));
                    break;
                default: assert(0); break;
                }
            }


    return textureName;
}

unsigned InitTexture(unsigned& texture) {

    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO); //size, out bufferID

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // 0. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // type, bufferID
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);// type, size, data, usage
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // type, bufferID
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);// type, size, data, usage
    // 1. then set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //index(location), size(dim), type, normalize, stride(next point), pointer(offset of the first)
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // color attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //glGenTextures(1, &texture); //num texture, ID
    //glBindTexture(GL_TEXTURE_2D, texture);//type data
    //// set the texture wrapping/filtering options (on the currently bound texture object)
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //// load and generate the texture
    //stbi_set_flip_vertically_on_load(true);
    //int width, height, nrChannels;
    //unsigned char* data = stbi_load("../data/image.png", &width, &height, &nrChannels, 0);
    //if (data)
    //{
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);//type, level, internalFormat, width, height, border, format, type, data
    //    glGenerateMipmap(GL_TEXTURE_2D);
    //}
    //else
    //{
    //    std::cout << "Failed to load texture" << std::endl;
    //}
    //stbi_image_free(data);

    return VAO;
}

void DisplayTexture(Shader shader, unsigned VAO, unsigned texture) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glUseProgram(shader.ID);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main(int argc, char** argv)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    Shader ourShader("../shaders/12_hello_texture/texture.vert", "../shaders/12_hello_texture/texture.frag");

    //ConvertToDds("../data/12_hello_texture/texture.dds", "../data/12_hello_texture/textureConverted");
    unsigned texture;
    unsigned VAO = InitTexture(texture);
    texture = CreateTexture("../data/images/image.dds");
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // rendering commands here
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        DisplayTexture(ourShader, VAO, texture);

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}