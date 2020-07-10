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
#include <easy/profiler.h>

#include "shader.h"
#include "camera.h"
#include "cube.h"
#include "model.h"
#include <texture.h>


class HelloShadowsProgram : public RenderProgram
{
public:
	void Init() override
	{
		cube_.Init();
		lightCube_.Init();
		quadGround_.Init();
		cubeMaps_.Init();
		model_.loadModel("../data/models/enemy.obj");
		textureGroundDiffuse_.Init("../data/materials/kitchen_wood/kitchen_wood_diff.jpg");
		textureGroundSpecular_.Init("../data/materials/kitchen_wood/kitchen_wood_rough.jpg");
		textureCubeDiffuse_.Init("../data/materials/light-gold/lightgold_albedo.png");
		textureCubeSpecular_.Init("../data/materials/light-gold/lightgold_roughness.png");

		shaderCube_ = Shader("../shaders/112_hello_shadows/cube.vert",
			"../shaders/112_hello_shadows/cube.frag");
		shaderModel_ = Shader("../shaders/112_hello_shadows/model.vert",
			"../shaders/112_hello_shadows/model.frag");
		shaderGround_ = Shader("../shaders/112_hello_shadows/quad.vert",
			"../shaders/112_hello_shadows/quad.frag");
		shaderLight_ = Shader("../shaders/112_hello_shadows/light.vert",
			"../shaders/112_hello_shadows/light.frag");
		shaderCubeMaps_ = Shader("../shaders/112_hello_shadows/cubeMaps.vert",
			"../shaders/112_hello_shadows/cubeMaps.frag");
		cubeMapsTexture_ = LoadCubemap(cubeMapsPath_);

		// shader configuration
		// --------------------
		shaderCube_.Use();
		shaderCube_.SetInt("material.diffuse", 0);
		shaderCube_.SetInt("material.specular", 1);

		// shader configuration
		// --------------------
		shaderGround_.Use();
		shaderGround_.SetInt("material.diffuse", 0);
		shaderGround_.SetInt("material.specular", 1);
		camera_.Position = glm::vec3(0.0f, 3.0f, 3.0f);
	}

	void Update(seconds dt) override
	{
		ProcessInput(dt);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now

		glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		{
			shaderCube_.Use();
			// pass projection matrix to shader (note that in this case it could change every frame)
			glm::mat4 projection = glm::perspective(glm::radians(camera_.Zoom),
				(float)width_ / (float)height_, 0.1f, 100.0f);
			shaderCube_.SetMat4("projection", projection);

			// camera/view transformation
			glm::mat4 view = camera_.GetViewMatrix();
			shaderCube_.SetMat4("view", view);


			glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
			glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

			shaderCube_.SetInt("light.type", static_cast<int>(lightType_));
			if (lightType_ == LightType::FLASHLIGHT || lightType_ == LightType::SPOTLIGHT)
			{
				shaderCube_.SetVec3("light.position", camera_.Position);
				//std::cout << std::to_string(camera_.Position.x) + " " + std::to_string(camera_.Position.y) + " " + std::to_string(camera_.Position.z) << std::endl;
				shaderCube_.SetVec3("light.direction", camera_.Front);
			}
			else
			{
				shaderCube_.SetVec3("light.position", lightPosition_);
				//std::cout << std::to_string(lightPosition_.x) + " " + std::to_string(lightPosition_.y) + " " + std::to_string(lightPosition_.z) << std::endl;
				shaderCube_.SetVec3("light.direction", lightDirection_);
			}
			shaderCube_.SetFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
			shaderCube_.SetFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
			shaderCube_.SetFloat("light.constant", 1.0f);
			shaderCube_.SetFloat("light.linear", 0.09f);
			shaderCube_.SetFloat("light.quadratic", 0.032f);
			shaderCube_.SetVec3("light.ambient", ambientColor);
			shaderCube_.SetVec3("light.diffuse", diffuseColor);
			shaderCube_.SetVec3("light.specular", 1.0f, 1.0f, 1.0f);

			// bind diffuse map
			glActiveTexture(GL_TEXTURE0);
			textureCubeDiffuse_.BindTexture();
			// bind specular map
			glActiveTexture(GL_TEXTURE1);
			textureCubeSpecular_.BindTexture();

			shaderCube_.SetFloat("material.shininess", 32.0f);
			
			shaderCube_.SetVec3("viewPos", camera_.Position);

			cube_.BindVao();
			for (unsigned int i = 0; i < 2; i++)
			{
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, cubePosition_[i]);
				shaderCube_.SetMat4("model", model);
				cube_.Render();
			}
		}
		{
			shaderGround_.Use();
			// pass projection matrix to shader (note that in this case it could change every frame)
			glm::mat4 projection = glm::perspective(glm::radians(camera_.Zoom),
				(float)width_ / (float)height_, 0.1f, 100.0f);
			shaderGround_.SetMat4("projection", projection);

			// camera/view transformation
			glm::mat4 view = camera_.GetViewMatrix();
			shaderGround_.SetMat4("view", view);


			glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
			glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

			shaderGround_.SetInt("light.type", static_cast<int>(lightType_));
			if (lightType_ == LightType::FLASHLIGHT || lightType_ == LightType::SPOTLIGHT)
			{
				shaderGround_.SetVec3("light.position", camera_.Position);
				//std::cout << std::to_string(camera_.Position.x) + " " + std::to_string(camera_.Position.y) + " " + std::to_string(camera_.Position.z) << std::endl;
				shaderGround_.SetVec3("light.direction", camera_.Front);
			}
			else
			{
				shaderGround_.SetVec3("light.position", lightPosition_);
				//std::cout << std::to_string(lightPosition_.x) + " " + std::to_string(lightPosition_.y) + " " + std::to_string(lightPosition_.z) << std::endl;
				shaderGround_.SetVec3("light.direction", lightDirection_);
			}
			shaderGround_.SetFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
			shaderGround_.SetFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
			shaderGround_.SetFloat("light.constant", 1.0f);
			shaderGround_.SetFloat("light.linear", 0.09f);
			shaderGround_.SetFloat("light.quadratic", 0.032f);
			shaderGround_.SetVec3("light.ambient", ambientColor);
			shaderGround_.SetVec3("light.diffuse", diffuseColor);
			shaderGround_.SetVec3("light.specular", 1.0f, 1.0f, 1.0f);

			// bind diffuse map
			glActiveTexture(GL_TEXTURE0);
			textureGroundDiffuse_.BindTexture();
			// bind specular map
			glActiveTexture(GL_TEXTURE1);
			textureGroundSpecular_.BindTexture();
			
			shaderGround_.SetFloat("material.shininess", 32.0f);
			shaderGround_.SetVec3("viewPos", camera_.Position);

			quadGround_.BindVao();

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, quadPosition_);
			model = glm::rotate(model, -glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(20.0f));
			shaderGround_.SetMat4("model", model);
			quadGround_.Render();
		}
		{
			shaderModel_.Use();
			// pass projection matrix to shader (note that in this case it could change every frame)
			glm::mat4 projection = glm::perspective(glm::radians(camera_.Zoom),
				(float)width_ / (float)height_, 0.1f, 100.0f);
			shaderModel_.SetMat4("projection", projection);

			// camera/view transformation
			glm::mat4 view = camera_.GetViewMatrix();
			shaderModel_.SetMat4("view", view);


			glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
			glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

			shaderModel_.SetInt("light.type", static_cast<int>(lightType_));
			if (lightType_ == LightType::FLASHLIGHT || lightType_ == LightType::SPOTLIGHT)
			{
				shaderModel_.SetVec3("light.position", camera_.Position);
				//std::cout << std::to_string(camera_.Position.x) + " " + std::to_string(camera_.Position.y) + " " + std::to_string(camera_.Position.z) << std::endl;
				shaderModel_.SetVec3("light.direction", camera_.Front);
			}
			else
			{
				shaderModel_.SetVec3("light.position", lightPosition_);
				//std::cout << std::to_string(lightPosition_.x) + " " + std::to_string(lightPosition_.y) + " " + std::to_string(lightPosition_.z) << std::endl;
				shaderModel_.SetVec3("light.direction", lightDirection_);
			}
			shaderModel_.SetFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
			shaderModel_.SetFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
			shaderModel_.SetFloat("light.constant", 1.0f);
			shaderModel_.SetFloat("light.linear", 0.09f);
			shaderModel_.SetFloat("light.quadratic", 0.032f);
			shaderModel_.SetVec3("light.ambient", ambientColor);
			shaderModel_.SetVec3("light.diffuse", diffuseColor);
			shaderModel_.SetVec3("light.specular", 1.0f, 1.0f, 1.0f);
			
			shaderModel_.SetFloat("material.shininess", 32.0f);
			shaderModel_.SetVec3("viewPos", camera_.Position);

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, modelPosition_);
			model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));	// it's a bit too big for our scene, so scale it down
			shaderModel_.SetMat4("model", model);
			model_.Draw(shaderModel_);
		}
		if (lightType_ == LightType::POINT || lightType_ == LightType::NONE)
		{
			shaderLight_.Use();
			// pass projection matrix to shader (note that in this case it could change every frame)
			glm::mat4 projection = glm::perspective(glm::radians(camera_.Zoom),
				(float)width_ / (float)height_, 0.1f, 100.0f);
			shaderLight_.SetMat4("projection", projection);

			// camera/view transformation
			glm::mat4 view = camera_.GetViewMatrix();
			shaderLight_.SetMat4("view", view);

			shaderLight_.SetVec3("lightColor", lightColor);
			lightCube_.BindVao();
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, lightPosition_);
			model = glm::scale(model, glm::vec3(0.2f));
			shaderLight_.SetMat4("model", model);
			lightCube_.Render();
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
		shaderGround_.Destroy();
		shaderModel_.Destroy();
		cube_.Destroy();
		quadGround_.Destroy();
		textureCubeDiffuse_.Destroy();
		textureCubeSpecular_.Destroy();
		textureGroundDiffuse_.Destroy();
		textureGroundSpecular_.Destroy();
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

		if (keys[SDL_SCANCODE_LSHIFT])
		{
			camera_.MovementSpeed = SPEED * 3.0f;
		} else
		{
			camera_.MovementSpeed = SPEED;
		}
		
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


	enum class LightType
	{
		DIRECTIONNAL = 0,
		POINT = 1,
		FLASHLIGHT = 2,
		SPOTLIGHT = 3,
		NONE = 4
	};

	LightType lightType_ = LightType::DIRECTIONNAL;

	glm::vec3 lightPosition_ = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 lightDirection_ = glm::vec3(-0.2f, -1.0f, -0.3f);

	glm::vec3 cubePosition_[2] =
	{
		glm::vec3(5.0f, 0.5f, 0.0f),
		glm::vec3(0.0f, 0.5f, -5.0f)
	};

	glm::vec3 quadPosition_ = glm::vec3(0.0f, 0.0f, 0.0f);
	
	glm::vec3 modelPosition_ = glm::vec3(0.0f, 0.0f, 0.0f);

	Shader shaderCube_;
	Shader shaderGround_;
	Shader shaderModel_;
	Shader shaderLight_;
	Shader shaderCubeMaps_;

	RenderCube cube_;
	RenderCube lightCube_;
	RenderQuad quadGround_;
	RenderCubeMaps cubeMaps_;
	unsigned cubeMapsTexture_;
	std::vector<std::string> cubeMapsPath_ =
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

	Texture textureGroundDiffuse_;
	Texture textureGroundSpecular_;
	Texture textureCubeDiffuse_;
	Texture textureCubeSpecular_;

	Model model_;
};

int main(int argc, char** argv)
{
	EASY_PROFILER_ENABLE;
	Engine engine;
	HelloShadowsProgram renderProgram;
	engine.AddProgram(&renderProgram);
	engine.Init();
	engine.StartLoop();
	profiler::dumpBlocksToFile("../profile/111_hello_instancing.prof");
	return EXIT_SUCCESS;
}
