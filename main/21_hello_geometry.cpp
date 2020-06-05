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


class HelloGeometryProgram : public RenderProgram
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
		std::string geometryCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		std::ifstream gShaderFile;
		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// open files
			vShaderFile.open("../shaders/21_hello_geometry/cube.vert");
			fShaderFile.open("../shaders/21_hello_geometry/cube.frag");
			gShaderFile.open("../shaders/21_hello_geometry/cube.geom");
			std::stringstream vShaderStream, fShaderStream, gShaderStream;
			// read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			gShaderStream << gShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			gShaderFile.close();
			// convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
			geometryCode = gShaderStream.str();
		}
		catch (std::ifstream::failure& e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();
		const char* gShaderCode = geometryCode.c_str();

		GLuint vertexShader = createShader(GL_VERTEX_SHADER, vShaderCode);
		GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fShaderCode);
		GLuint geometryShader = createShader(GL_GEOMETRY_SHADER, gShaderCode);

		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		//glAttachShader(shaderProgram, geometryShader);
		glLinkProgram(shaderProgram);
		glUseProgram(shaderProgram);

		glGenBuffers(1, &VBO);
		float points[] = {
			//  Coordinates  Color             Sides
				-0.45f,  0.45f, 1.0f, 0.0f, 0.0f,  4.0f,
				 0.45f,  0.45f, 0.0f, 1.0f, 0.0f,  8.0f,
				 0.45f, -0.45f, 0.0f, 0.0f, 1.0f, 16.0f,
				-0.45f, -0.45f, 1.0f, 1.0f, 0.0f, 32.0f
		};

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
		// Création du VAO
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// Spécification de l'agencement des données
		GLint posAttrib = glGetAttribLocation(shaderProgram, "pos");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE,
			6 * sizeof(float), 0);

		GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
		glEnableVertexAttribArray(colAttrib);
		glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,
			6 * sizeof(float), (void*)(2 * sizeof(float)));

		GLint sidesAttrib = glGetAttribLocation(shaderProgram, "sides");
		glEnableVertexAttribArray(sidesAttrib);
		glVertexAttribPointer(sidesAttrib, 1, GL_FLOAT, GL_FALSE,
			6 * sizeof(float), (void*)(5 * sizeof(float)));
	}

	void Update(seconds dt) override
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);
		glDrawArrays(GL_POINTS, 0, 4);
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

	unsigned VAO;
	unsigned VBO;
	unsigned shaderProgram;
};

int main(int argc, char** argv)
{
	Engine engine;
	HelloGeometryProgram renderProgram;
	engine.AddProgram(&renderProgram);
	engine.Init();
	engine.StartLoop();
	return EXIT_SUCCESS;
}
