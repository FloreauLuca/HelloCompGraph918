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


#include "shader.h"
#include "camera.h"
#include "cube.h"
#include "texture.h"


class HelloStencilProgram : public RenderProgram
{
public:
	void Init() override
	{
		cube_.Init();
		cubeOutline_.Init();
		texture_.Init("../data/materials/leatherDiffuse.jpg");

		shaderCube_ = Shader("../shaders/17_hello_stencil/cube.vert",
		                "../shaders/17_hello_stencil/cube.frag");
		shaderOutline_ = Shader("../shaders/17_hello_stencil/outline.vert",
			"../shaders/17_hello_stencil/outline.frag");
		
	}

	void Update(seconds dt) override
	{
		ProcessInput(dt);
		glEnable(GL_DEPTH_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glDepthFunc(GL_LESS);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		glDisable(GL_DEPTH_TEST);
		for (int i = 0; i < 4; i++)
		{
			// don't forget to enable shader before setting uniforms
			shaderOutline_.Use();

			cubeOutline_.BindVao();
			// view/projection transformations
			glm::mat4 projection = glm::perspective(glm::radians(camera_.Zoom), (float)width_ / (float)height_, 0.1f, 100.0f);
			glm::mat4 view = camera_.GetViewMatrix();
			shaderOutline_.SetMat4("projection", projection);
			shaderOutline_.SetMat4("view", view);
			shaderOutline_.SetVec3("color", glm::vec3(1.0f));

			// render the loaded model
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePosition_[i]);
			model = glm::scale(model, glm::vec3(1.2f));
			shaderOutline_.SetMat4("model", model);
			cubeOutline_.Render();
			glBindVertexArray(0);
		}
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glEnable(GL_DEPTH_TEST);
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
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
	}

	void Destroy() override
	{
		shaderCube_.Destroy();
		shaderOutline_.Destroy();
		cube_.Destroy();
		cubeOutline_.Destroy();
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
	
	Shader shaderOutline_;
	Shader shaderCube_;

	RenderCube cube_;
	RenderCube cubeOutline_;

	Camera camera_;
	bool firstMouse_ = true;
	bool rightMouse_ = false;
	float lastX;
	float lastY;
	float width_ = 800.0f;
	float height_ = 600.0f;

	Texture texture_;
};

int main(int argc, char** argv)
{
	Engine engine;
	HelloStencilProgram renderProgram;
	engine.AddProgram(&renderProgram);
	engine.Init();
	engine.StartLoop();
	return EXIT_SUCCESS;
}
