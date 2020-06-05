//
// Created by unite on 12/05/2020.
//

#include <fstream>

#include <render_program.h>
#include <engine.h>
#include <iostream>
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <sstream>


#include "shader.h"
#include "camera.h"
#include "cube.h"
#include "model.h"


class HelloTessellation : public RenderProgram
{
public:
	GLuint createShader(GLenum type, const GLchar* src)
	{
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);
		return shader;
	}

	void Init() override
	{
		//texture = CreateTexture("../data/image.dds");
		//cube_.Init();std::string vertexCode;
		std::string vertexCode;
		std::string fragmentCode;
		std::string teseCode;
		std::string tescCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		std::ifstream teShaderFile;
		std::ifstream tcShaderFile;
		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		teShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		tcShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// open files
			vShaderFile.open("../shaders/22_hello_tessellation/cube.vert");
			fShaderFile.open("../shaders/22_hello_tessellation/cube.frag");
			teShaderFile.open("../shaders/22_hello_tessellation/cube.tese");
			tcShaderFile.open("../shaders/22_hello_tessellation/cube.tesc");
			std::stringstream vShaderStream, fShaderStream, gShaderStream, teShaderStream, tcShaderStream;
			// read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			tcShaderStream << tcShaderFile.rdbuf();
			teShaderStream << teShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			tcShaderFile.close();
			teShaderFile.close();
			// convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
			tescCode = tcShaderStream.str();
			teseCode = teShaderStream.str();
		}
		catch (std::ifstream::failure& e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();
		const char* teseShaderCode = teseCode.c_str();
		const char* tescShaderCode = tescCode.c_str();

		GLuint vertexShader = createShader(GL_VERTEX_SHADER, vShaderCode);
		GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fShaderCode);
		GLuint tescShader = createShader(GL_TESS_CONTROL_SHADER, tescShaderCode);
		GLuint teseShader = createShader(GL_TESS_EVALUATION_SHADER, teseShaderCode);

		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glAttachShader(shaderProgram, tescShader);
		glAttachShader(shaderProgram, teseShader);
		glLinkProgram(shaderProgram);
		glUseProgram(shaderProgram);/*
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glDeleteShader(tescShader);
		glDeleteShader(teseShader);*/

		// Init
		glm::mat4 p = glm::perspective(glm::radians(45.0f), 1600.0f / 900.0f, 0.1f, 1000.0f);
		// look at <0,0,0> from <20,20,20>
		glm::mat4 v = glm::lookAt(glm::vec3(20.0f), glm::vec3(0.0f), glm::vec3(0, 1, 0));
		glm::mat4 m = glm::mat4(1.0f);
		mvp = p * v * m;

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(3);

		vertices.push_back(glm::vec3(-0.5f, 0.0f, -0.5f));
		vertices.push_back(glm::vec3(0.5f, 0.0f, -0.5f));
		vertices.push_back(glm::vec3(0.5f, 0.0f, 0.5f));
		vertices.push_back(glm::vec3(-0.5f, 0.0f, 0.5f));

		// VAO
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// program

		// mvp uniform
		//matrix = glGetUniformLocation(shaderProgram, "mvp");

		// Vertex Buffer
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Element Buffer
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


	}

	void Update(seconds dt) override
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);

		//glUniformMatrix4fv(matrix, 1, GL_FALSE, &mvp[0][0]);

		glEnableVertexAttribArray(0);

		glPatchParameteri(GL_PATCH_VERTICES, 4);
		glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, (void*)0);
	}

	void Destroy() override
	{
		glDeleteShader(shaderProgram);
	}

	void OnEvent(const SDL_Event& event) override
	{
	}

	void UpdateUi(seconds dt) override
	{
	}

private:

	void ProcessInput(seconds dt)
	{
	}

	unsigned matrix;
	unsigned VAO;
	unsigned VBO;
	unsigned EBO;
	unsigned shaderProgram;

	std::vector<unsigned int> indices;
	std::vector<glm::vec3> vertices;

	glm::mat4 mvp;
	
	
	float outerLevels_;
	float innerLevels_;
};

int main(int argc, char** argv)
{
	Engine engine;
	HelloTessellation renderProgram;
	engine.AddProgram(&renderProgram);
	engine.Init();
	engine.StartLoop();
	return EXIT_SUCCESS;
}
