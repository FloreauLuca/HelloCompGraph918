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


class HelloLightProgram : public RenderProgram
{
public:
	void Init() override
	{
		//texture = CreateTexture("../data/image.dds");
		cube_.Init();
		lightCube_.Init();
		
		shader_ = Shader("../shaders/demo_light/cube.vert",
		                 "../shaders/demo_light/cube.frag");

		shaderLight_ = Shader("../shaders/demo_light/light.vert",
		                      "../shaders/demo_light/light.frag");
	}

	void Update(seconds dt) override
	{
		timeSince_ += dt.count();
		ProcessInput(dt);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::vec3 lightPosition[3] =
		{
			lightPosition_[0],
			lightPosition_[1],
			lightPosition_[2],
		};
		cubePosition_.z = sin(timeSince_) * 15 - 12;
		{
			shader_.Use();
			// pass projection matrix to shader (note that in this case it could change every frame)
			glm::mat4 projection = glm::perspective(glm::radians(camera_.Zoom),
			                                        (float)width_ / (float)height_, 0.1f, 100.0f);
			shader_.SetMat4("projection", projection);

			// camera/view transformation
			glm::mat4 view = camera_.GetViewMatrix();
			shader_.SetMat4("view", view);

			{
				glm::vec3 lightColor = lightColor_[0];
				glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
				glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

				shader_.SetVec3("flashLight[0].position", lightPosition[0]);
				shader_.SetVec3("flashLight[0].direction", lightDirection_);
				shader_.SetFloat("flashLight[0].cutOff", glm::cos(glm::radians(12.5f)));
				shader_.SetVec3("flashLight[0].ambient", ambientColor);
				shader_.SetVec3("flashLight[0].diffuse", diffuseColor);
				shader_.SetVec3("flashLight[0].specular", glm::vec3(0.0f));
			}
			{
				glm::vec3 lightColor = lightColor_[1];
				glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
				glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

				shader_.SetVec3("flashLight[1].position", lightPosition[1]);
				shader_.SetVec3("flashLight[1].direction", lightDirection_);
				shader_.SetFloat("flashLight[1].cutOff", glm::cos(glm::radians(12.5f)));
				shader_.SetVec3("flashLight[1].ambient", ambientColor);
				shader_.SetVec3("flashLight[1].diffuse", diffuseColor);
				shader_.SetVec3("flashLight[1].specular", glm::vec3(0.0f));
			}
			{
				glm::vec3 lightColor = lightColor_[2];
				glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
				glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

				shader_.SetVec3("flashLight[2].position", lightPosition[2]);
				shader_.SetVec3("flashLight[2].direction", lightDirection_);
				shader_.SetFloat("flashLight[2].cutOff", glm::cos(glm::radians(12.5f)));
				shader_.SetVec3("flashLight[2].ambient", ambientColor);
				shader_.SetVec3("flashLight[2].diffuse", diffuseColor);
				shader_.SetVec3("flashLight[2].specular", glm::vec3(0.0f));
			}
			shader_.SetVec3("material.ambient", glm::vec3(1.0f));
			shader_.SetVec3("material.diffuse", glm::vec3(1.0f));
			shader_.SetVec3("material.specular", glm::vec3(1.0f));
			shader_.SetFloat("material.shininess", 32.0f);
			shader_.SetVec3("viewPos", camera_.Position);

			cube_.BindVao();
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePosition_);
			model = glm::scale(model, cubeScale_);
			shader_.SetMat4("model", model);
			cube_.Render();
		}
		{
			shaderLight_.Use();
			// pass projection matrix to shader (note that in this case it could change every frame)
			glm::mat4 projection = glm::perspective(glm::radians(camera_.Zoom),
			                                        (float)width_ / (float)height_, 0.1f, 100.0f);
			shaderLight_.SetMat4("projection", projection);

			// camera/view transformation
			glm::mat4 view = camera_.GetViewMatrix();
			shaderLight_.SetMat4("view", view);

			for (int i = 0; i < 3; i++)
			{
				shaderLight_.SetVec3("lightColor", lightColor_[i]);
				lightCube_.BindVao();

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, lightPosition[i]);
				model = glm::scale(model, glm::vec3(0.2f));
				shaderLight_.SetMat4("model", model);
				lightCube_.Render();
			}
		}
	}

	void Destroy() override
	{
		shader_.Destroy();
		shaderLight_.Destroy();
		cube_.Destroy();
		lightCube_.Destroy();
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
		float direction[3] = { lightDirection_.x, lightDirection_.y, lightDirection_.z };
		if (ImGui::DragFloat3("Direction",direction))
		{
			lightDirection_.x = direction[0];
			lightDirection_.y = direction[1];
			lightDirection_.z = direction[2];
		}

		float position[3] = { cubePosition_.x, cubePosition_.y, cubePosition_.z };
		if (ImGui::DragFloat3("Position", position))
		{
			cubePosition_.x = position[0];
			cubePosition_.y = position[1];
			cubePosition_.z = position[2];
		}
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


	float timeSince_ = 800.0f;

	glm::vec3 lightPosition_[3] =
	{
		glm::vec3(-1.0f, 0, 5.0f),
		glm::vec3(1.0f, 0, 5.0f),
		glm::vec3(0.0f, -1.732, 5.0f)
	};
	glm::vec3 lightDirection_ = glm::vec3(0.0f, 0.0f, -1.0f);

	glm::vec3 lightColor_[3] =
	{
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	};

	glm::vec3 cubePosition_ = glm::vec3(0.0f, 0.0f, -5.0f);
	glm::vec3 cubeScale_ = glm::vec3(10.0f, 10.0f, 1.0f);
	
	Shader shader_;
	Shader shaderLight_;

	RenderCube cube_;
	RenderCube lightCube_;

	Camera camera_;
	bool firstMouse_ = true;
	bool rightMouse_ = false;
	float lastX;
	float lastY;
	float width_ = 800.0f;
	float height_ = 600.0f;
};

int main(int argc, char** argv)
{
	Engine engine;
	HelloLightProgram renderProgram;
	engine.AddProgram(&renderProgram);
	engine.Init();
	engine.StartLoop();
	return EXIT_SUCCESS;
}
