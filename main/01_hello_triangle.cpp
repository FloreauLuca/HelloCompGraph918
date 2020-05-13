//
// Created by unite on 12/05/2020.
//

#include <fstream>

#include <render_program.h>
#include <engine.h>
#include <iostream>
#include <GL/glew.h>


class HelloTriangleProgram : public RenderProgram
{
public:
    void Init() override
    {
        float vertices[] = {
		    -0.5f, -0.5f, 0.0f, //bottom left
		     0.5f, -0.5f, 0.0f, //bottom right
		     0.0f,  0.5f, 0.0f //top
        };
        unsigned int VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
    	
        unsigned int VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

    	//Compile and link shaders

        char infoLog[512];
        int success;
        unsigned int vertexShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        const auto* vertexPath = "shaders/01_hello_triangle/triangle.vert";
        std::ifstream vertexShaderFile(vertexPath);
        if (vertexShaderFile.is_open())
        {
            std::string fileContent((std::istreambuf_iterator<char>(vertexShaderFile)),
                std::istreambuf_iterator<char>());
            vertexShaderFile.close();
            auto* fileContentPtr = fileContent.c_str();
            glShaderSource(vertexShader, 1, &fileContentPtr, NULL);
            glCompileShader(vertexShader);
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            }
        }
        else
        {
            std::cerr << "[Error] Could not open vertex shader file: " << vertexPath << '\n';
        }

        unsigned int fragShader;
        fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        const auto* fragPath = "shaders/01_hello_triangle/triangle.frag";
        std::ifstream fragShaderFile(fragPath);
        if (fragShaderFile.is_open())
        {
            std::string fileContent((std::istreambuf_iterator<char>(fragShaderFile)),
                std::istreambuf_iterator<char>());
            fragShaderFile.close();
            auto* fileContentPtr = fileContent.c_str();
            glShaderSource(fragShader, 1, &fileContentPtr, NULL);
            glCompileShader(fragShader);
            glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
            }
        }
        else
        {
            std::cerr << "[Error] Could not open frag shader file: " << fragPath << '\n';
        }
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragShader);
        glLinkProgram(shaderProgram);
        //Check if shader program was linked correctly
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {

            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cerr << "[Error] Shader program with vertex: " << vertexPath << " and fragment: " << fragPath << " LINK_FAILED\n" << infoLog;
            return;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragShader);
    }

    void Update(seconds dt) override
    {
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void Destroy() override
    {
        glDeleteProgram(shaderProgram);
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }

    void OnEvent(const SDL_Event &event) override
    {

    }

    void UpdateUi(seconds dt) override {

    }
private:
    unsigned VAO;
    unsigned VBO;

    unsigned shaderProgram;
};

int main(int argc, char** argv)
{
    Engine engine;
    HelloTriangleProgram renderProgram;
    engine.AddProgram(&renderProgram);
    engine.Init();
    engine.StartLoop();
    return EXIT_SUCCESS;
}