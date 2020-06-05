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
#include <stb_image.h>



#include "shader.h"
#include "camera.h"
#include "cube.h"
#include "texture.h"


class HelloCubeMapsProgram : public RenderProgram
{
public:
	void Init() override
	{
		cube_.Init();
		cubeMaps_.Init();
		quadRed_.Init();
		texture_.Init("../data/110_hello_cubemaps/leatherDiffuse.jpg");
		textureBlue_.Init("../data/110_hello_cubemaps/BlueBlend.png");
		textureGreen_.Init("../data/110_hello_cubemaps/GreenBlend.png");
		textureRed_.Init("../data/110_hello_cubemaps/RedBlend.png");

		shaderCube_ = Shader("../shaders/110_hello_cubemaps/cube.vert",
			"../shaders/110_hello_cubemaps/cube.frag");
		shaderBlend_ = Shader("../shaders/110_hello_cubemaps/blend.vert",
			"../shaders/110_hello_cubemaps/blend.frag");
		shaderCubeMaps_ = Shader("../shaders/110_hello_cubemaps/cubeMaps.vert",
			"../shaders/110_hello_cubemaps/cubeMaps.frag");
		cubeMapsTexture_ = LoadCubemap(cubeMapsPath_);
	}

	void Update(seconds dt) override
	{
		ProcessInput(dt);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now

		
		for (int i = 0; i < 4; i++)
		{
			// don't forget to enable shader before setting uniforms
			shaderCube_.Use();

			cube_.BindVao();
			shaderCube_.SetInt("ourTexture", 0);
			glActiveTexture(GL_TEXTURE0);
			texture_.BindTexture();
			// view/projection transformations
			glm::mat4 projection = glm::perspective(glm::radians(camera_.Zoom), (float)width_ / (float)height_, 0.1f, 100.0f);
			glm::mat4 view = camera_.GetViewMatrix();
			shaderCube_.SetMat4("projection", projection);
			shaderCube_.SetMat4("view", view);
			shaderCube_.SetVec3("color", glm::vec3(0.2, 0.5, 0.5));

			// render the loaded model
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePosition_[i]); // translate it down so it's at the center of the scene
			shaderCube_.SetMat4("model", model);
			cube_.Render();
			glBindVertexArray(0);


		}
		for (int i = 0; i < 4; i++)
		{
			// don't forget to enable shader before setting uniforms
			shaderBlend_.Use();

			quadRed_.BindVao();
			shaderBlend_.SetInt("ourTexture", i % 3);
			glActiveTexture(GL_TEXTURE0);
			textureRed_.BindTexture();
			glActiveTexture(GL_TEXTURE1);
			textureBlue_.BindTexture();
			glActiveTexture(GL_TEXTURE2);
			textureGreen_.BindTexture();
			 //view/projection transformations
			glm::mat4 projection = glm::perspective(glm::radians(camera_.Zoom), (float)width_ / (float)height_, 0.1f, 100.0f);
			glm::mat4 view = camera_.GetViewMatrix();
			shaderBlend_.SetMat4("projection", projection);
			shaderBlend_.SetMat4("view", view);
			shaderBlend_.SetVec3("color", glm::vec3(0.2, 0.5, 0.5));

			// render the loaded model
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, quadPosition_[i]); // translate it down so it's at the center of the scene
			model = glm::rotate(model, i * glm::half_pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
			shaderBlend_.SetMat4("model", model);
			quadRed_.Render();
			glBindVertexArray(0);


		}

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		shaderCubeMaps_.Use();
		cubeMaps_.BindVao();
		glm::mat4 projection = glm::perspective(glm::radians(camera_.Zoom), (float)width_ / (float)height_, 0.1f, 100.0f);
		glm::mat4 view = glm::mat4(glm::mat3(camera_.GetViewMatrix()));
		shaderCubeMaps_.SetMat4("projection", projection);
		shaderCubeMaps_.SetMat4("view", view);
		shaderCubeMaps_.SetInt("skybox", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapsTexture_);
		cubeMaps_.Render();
		glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default
	}

	void Destroy() override
	{
		shaderCube_.Destroy();
		shaderBlend_.Destroy();
		shaderCubeMaps_.Destroy();
		cube_.Destroy();
		quadRed_.Destroy();
		cubeMaps_.Destroy();
	}

	void OnEvent(const SDL_Event& event) override
	{
		if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			if (event.button.button == SDL_BUTTON_RIGHT)
			{
				rightMouse_ = true;
				lastX = event.motion.x;
				lastY = event.motion.y;
				firstMouse_ = false;
			}
		}

		if (event.type == SDL_MOUSEBUTTONUP)
		{
			if (event.button.button == SDL_BUTTON_RIGHT)
			{
				rightMouse_ = false;
			}
		}
		if (rightMouse_)
		{
			if (event.type == SDL_MOUSEMOTION)
			{
				if (firstMouse_)
				{
					lastX = event.motion.x;
					lastY = event.motion.y;
					firstMouse_ = false;
				}

				float xoffset = event.motion.x - lastX;
				float yoffset = lastY - event.motion.y;
				// reversed since y-coordinates go from bottom to top

				lastX = event.motion.x;
				lastY = event.motion.y;

				camera_.ProcessMouseMovement(xoffset, yoffset);
			}
		}
		if (event.type == SDL_WINDOWEVENT_RESIZED)
		{
			width_ = event.window.data1;
			height_ = event.window.data2;
		}
	}

	void UpdateUi(seconds dt) override
	{

	}

private:

	unsigned int LoadCubemap(std::vector<std::string> faces)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			stbi_set_flip_vertically_on_load(false);
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}
	
	void ProcessInput(seconds dt)
	{
		//Checking if keys are down
		const Uint8* keys = SDL_GetKeyboardState(NULL);
		if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
		{
			camera_.ProcessKeyboard(Camera_Movement::LEFT, dt.count());
		}

		if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
		{
			camera_.ProcessKeyboard(Camera_Movement::RIGHT, dt.count());
		}

		if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
		{
			camera_.ProcessKeyboard(Camera_Movement::FORWARD, dt.count());
		}

		if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
		{
			camera_.ProcessKeyboard(Camera_Movement::BACKWARD, dt.count());
		}
	}

	glm::vec3 cubePosition_[4] =
	{
		glm::vec3(5.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -5.0f),
		glm::vec3(0.0f, 0.0f, 5.0f),
		glm::vec3(-5.0f, 0.0f, 0.0f)
	};

	glm::vec3 quadPosition_[4] =
	{
		glm::vec3(0.0f, 0.0f, -2.0f),
		glm::vec3(-2.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 2.0f),
		glm::vec3(2.0f, 0.0f, 0.0f)
	};

	Shader shaderCube_;
	Shader shaderBlend_;
	Shader shaderCubeMaps_;

	RenderCube cube_;
	RenderQuad quadRed_;
	RenderCubeMaps cubeMaps_;
	unsigned cubeMapsTexture_;
	std::vector<std::string> cubeMapsPath_=
	{
		"../data/110_hello_cubemaps/wild/posx.jpg",
		"../data/110_hello_cubemaps/wild/negx.jpg",
		"../data/110_hello_cubemaps/wild/posy.jpg",
		"../data/110_hello_cubemaps/wild/negy.jpg",
		"../data/110_hello_cubemaps/wild/posz.jpg",
		"../data/110_hello_cubemaps/wild/negz.jpg",
	};

	Camera camera_;
	bool firstMouse_ = true;
	bool rightMouse_ = false;
	float lastX;
	float lastY;
	float width_ = 800.0f;
	float height_ = 600.0f;

	Texture texture_;
	Texture textureRed_;
	Texture textureBlue_;
	Texture textureGreen_;
};

int main(int argc, char** argv)
{
	Engine engine;
	HelloCubeMapsProgram renderProgram;
	engine.AddProgram(&renderProgram);
	engine.Init();
	engine.StartLoop();
	return EXIT_SUCCESS;
}
