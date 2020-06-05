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


class HelloFrameBufferProgram : public RenderProgram
{
public:
	void Init() override
	{
		cube_.Init();
		cubeOutline_.Init();
		quadRed_.Init();
		screen_.Init();
		texture_.Init("../data/19_hello_framebuffer/leatherDiffuse.jpg");
		textureBlue_.Init("../data/19_hello_framebuffer/BlueBlend.png");
		textureGreen_.Init("../data/19_hello_framebuffer/GreenBlend.png");
		textureRed_.Init("../data/19_hello_framebuffer/RedBlend.png");

		shaderCube_ = Shader("../shaders/19_hello_framebuffer/cube.vert",
			"../shaders/19_hello_framebuffer/cube.frag");
		shaderOutline_ = Shader("../shaders/19_hello_framebuffer/outline.vert",
			"../shaders/19_hello_framebuffer/outline.frag");
		shaderBlend_ = Shader("../shaders/19_hello_framebuffer/blend.vert",
			"../shaders/19_hello_framebuffer/blend.frag");
		shaderPostProcess_ = Shader("../shaders/19_hello_framebuffer/postprocess_negative.vert",
			"../shaders/19_hello_framebuffer/postprocess_negative.frag");


		glGenFramebuffers(1, &FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);

		// generate texture
		glGenTextures(1, &processedTexture);
		glBindTexture(GL_TEXTURE_2D, processedTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		// attach it to currently bound framebuffer object
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, processedTexture, 0);

		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		/*
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glGenTextures(1, &processedTexture);
		glBindTexture(GL_TEXTURE_2D, processedTexture);

		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0,
			GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
		);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, processedTexture, 0);

		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
		*/
	}

	void Update(seconds dt) override
	{
		ProcessInput(dt);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CW);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// first pass
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
		glEnable(GL_DEPTH_TEST);
		
		//DrawScene
		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

		// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
		// clear all relevant buffers
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT);

		shaderPostProcess_.Use();
		shaderPostProcess_.SetInt("screenTexture", 0);
		screen_.BindVao();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, processedTexture);
		screen_.Render();
	}

	void Destroy() override
	{
		shaderCube_.Destroy();
		shaderOutline_.Destroy();
		shaderBlend_.Destroy();
		cube_.Destroy();
		cubeOutline_.Destroy();
		quadRed_.Destroy();
		shaderPostProcess_.Destroy();
		screen_.Destroy();
		glDeleteFramebuffers(1, &FBO);
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

	glm::vec3 quadPosition_[4] =
	{
		glm::vec3(0.0f, 0.0f, -2.0f),
		glm::vec3(-2.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 2.0f),
		glm::vec3(2.0f, 0.0f, 0.0f)
	};

	unsigned FBO;
	unsigned RBO;
	unsigned processedTexture;
	
	Shader shaderOutline_;
	Shader shaderCube_;
	Shader shaderBlend_;
	Shader shaderPostProcess_;

	RenderCube cube_;
	RenderCube cubeOutline_;
	RenderQuad quadRed_;
	RenderQuad screen_;

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
	HelloFrameBufferProgram renderProgram;
	engine.AddProgram(&renderProgram);
	engine.Init();
	engine.StartLoop();
	return EXIT_SUCCESS;
}
