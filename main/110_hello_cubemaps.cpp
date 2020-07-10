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
		texture_.Init("../data/images/PenguinHead.png");
		textureBlue_.Init("../data/images/BlueBlend.png");
		textureGreen_.Init("../data/images/GreenBlend.png");
		textureRed_.Init("../data/images/RedBlend.png");

		shaderRefraction_ = Shader("../shaders/110_hello_cubemaps/cubeRefraction.vert",
			"../shaders/110_hello_cubemaps/cubeRefraction.frag");
		shaderReflection_ = Shader("../shaders/110_hello_cubemaps/cubeReflection.vert",
			"../shaders/110_hello_cubemaps/cubeReflection.frag");
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
			shaderReflection_.Use();
			shaderReflection_.SetInt("skybox", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapsTexture_);
			shaderReflection_.SetVec3("cameraPos", camera_.Position);
			cube_.BindVao();
			// view/projection transformations
			glm::mat4 projection = glm::perspective(glm::radians(camera_.Zoom), (float)width_ / (float)height_, 0.1f, 100.0f);
			glm::mat4 view = camera_.GetViewMatrix();
			shaderReflection_.SetMat4("projection", projection);
			shaderReflection_.SetMat4("view", view);
			shaderReflection_.SetVec3("color", glm::vec3(0.2, 0.5, 0.5));

			// render the loaded model
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePosition_[i]); // translate it down so it's at the center of the scene
			shaderReflection_.SetMat4("model", model);
			cube_.Render();
			glBindVertexArray(0);


		}
		for (int i = 0; i < 4; i++)
		{
			// don't forget to enable shader before setting uniforms
			shaderRefraction_.Use();
			shaderRefraction_.SetInt("skybox", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapsTexture_);
			shaderRefraction_.SetVec3("cameraPos", camera_.Position);
			quadRed_.BindVao();
			 //view/projection transformations
			glm::mat4 projection = glm::perspective(glm::radians(camera_.Zoom), (float)width_ / (float)height_, 0.1f, 100.0f);
			glm::mat4 view = camera_.GetViewMatrix();
			shaderRefraction_.SetMat4("projection", projection);
			shaderRefraction_.SetMat4("view", view);
			shaderRefraction_.SetVec3("color", glm::vec3(0.2, 0.2, 0.5));

			// render the loaded model
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, quadPosition_[i]); // translate it down so it's at the center of the scene
			model = glm::rotate(model, i * glm::half_pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
			shaderRefraction_.SetMat4("model", model);
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
		shaderRefraction_.Destroy();
		shaderReflection_.Destroy();
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
//
//	void GenerateCubemap()
//	{
//#ifdef EASY_PROFILE_USE
//		EASY_BLOCK("Generate Cubemap");
//#endif
//		glGenTextures(1, &envCubemap_);
//		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap_);
//		for (unsigned int i = 0; i < 6; ++i)
//		{
//			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
//		}
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
//		glCheckError();
//
//		glBindRenderbuffer(GL_RENDERBUFFER, captureRbo_);
//		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 512, 512);
//		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRbo_);
//		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, envCubemap_, 0);
//		glCheckError();
//
//		Camera3D captureCamera;
//		captureCamera.position = Vec3f::zero;
//		captureCamera.aspect = 1.0f;
//		captureCamera.fovY = degree_t(90.0f);
//		captureCamera.nearPlane = 0.1f;
//		captureCamera.farPlane = 10.0f;
//
//
//		equiToCubemap_.Bind();
//		equiToCubemap_.SetTexture("equirectangularMap", hdrTexture_, 0);
//		equiToCubemap_.SetMat4("projection", captureCamera.GenerateProjectionMatrix());
//		glViewport(0, 0, 512, 512);
//		glBindFramebuffer(GL_FRAMEBUFFER, captureFbo_);
//		for (unsigned int i = 0; i < 6; ++i)
//		{
//			captureCamera.WorldLookAt(viewDirs[i], upDirs[i]);
//			equiToCubemap_.SetMat4("view", captureCamera.GenerateViewMatrix());
//			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap_, 0);
//			glCheckFramebuffer();
//			glCheckError();
//			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//			skybox_.Draw();
//		}
//
//
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	}

	
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

	Shader shaderRefraction_;
	Shader shaderReflection_;
	Shader shaderCubeMaps_;

	RenderCube cube_;
	RenderQuad quadRed_;
	RenderCubeMaps cubeMaps_;
	unsigned cubeMapsTexture_;
	std::vector<std::string> cubeMapsPath_=
	{
		"../data/cubemaps/stairs/posx.jpg",
		"../data/cubemaps/stairs/negx.jpg",
		"../data/cubemaps/stairs/posy.jpg",
		"../data/cubemaps/stairs/negy.jpg",
		"../data/cubemaps/stairs/posz.jpg",
		"../data/cubemaps/stairs/negz.jpg",
	};

	Camera camera_;
	bool firstMouse_ = true;
	bool rightMouse_ = false;
	float lastX;
	float lastY;
	float width_ = 800.0f;
	float height_ = 600.0f;

	unsigned envCubemap_ = 0;
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
