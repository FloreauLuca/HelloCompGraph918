
//
// Created by Stephen Grosjean 02.06.2020
//

#include <fstream>
#include <render_program.h>
#include <engine.h>
#include <iostream>
#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class HelloTextureProgram : public RenderProgram
{
public:
    void Init() override
    {
        float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f, 0.5f, 0.0f
        };

        //Texture
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        float texCoords[8] = {
            1.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 1.0f
        };

        //Gen VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        //Gen VBO
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glBindBuffer(GL_ARRAY_BUFFER_ARB, VBO);

        //Vertex Shader
        vertexShader = glCreateShader(GL_VERTEX_SHADER);

        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);

        //Fragment Shader
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);

        //Shader program
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);



    }

    void Update(seconds dt) override
    {
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void Destroy() override
    {

    }

    void OnEvent(const SDL_Event& event) override
    {

    }

    void UpdateUi(seconds dt) override {

    }

private:
    unsigned int VBO;
    unsigned int VAO;
    unsigned int vertexShader;
    unsigned int fragmentShader;
    unsigned int shaderProgram;

    char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec2 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 TexCoord;\n"
        "void main(){\n"
        "   gl_Position = vec4(aPos, 0.0, 1.0);\n"
        "   TexCoord = aTexCoord;\n"
        "}\0";

    char* fragmentShaderSource = "#version 330 core \n"
        "precision mediump float;\n"
        "layout(location = 0) out vec4 FragColor;\n"
        "in vec2 TexCoord;\n"
        "uniform sampler2D ourTexture;\n"
        "void main() { \n"
        "   FragColor = texture(ourTexture, TexCoord); \n"
        "}\0";

    int width, height, nrChannels;
    unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
    unsigned int texture;
};

int main(int argc, char** argv)
{
    Engine engine;
    HelloTextureProgram renderProgram;
    engine.AddProgram(&renderProgram);
    engine.Init();
    engine.StartLoop();
    return EXIT_SUCCESS;
}