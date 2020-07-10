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
		textureDiffuse_.Init("../data/materials/leatherDiffuse.jpg");
		textureSpecular_.Init("../data/materials/leatherSpecular.jpg");

		shader_ = Shader("../shaders/15_hello_light/cube.vert",
		                "../shaders/15_hello_light/cube.frag");

		shaderLight_ = Shader("../shaders/15_hello_light/light.vert",
		                     "../shaders/15_hello_light/light.frag");
	}

	void Update(seconds dt) override
	{
		timeSince_ += dt.count();
		ProcessInput(dt);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CW);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthFunc(GL_LESS);

		glm::vec3 lightColor = glm::vec3(1.0f);
		//lightColor.x = sin(timeSince_ * 2.0f);
		//lightColor.y = sin(timeSince_ * 0.7f);
		//lightColor.z = sin(timeSince_ * 1.3f);
		//
		lightPosition_.x = sin(timeSince_ * 2.0f);
		lightPosition_.y = sin(timeSince_ * 0.7f);
		lightPosition_.z = -5.0f;
		{
			shader_.Use();
			shader_.SetInt("material.diffuse", 0);
			glActiveTexture(GL_TEXTURE0);
			textureDiffuse_.BindTexture();
			shader_.SetInt("material.specular", 1);
			glActiveTexture(GL_TEXTURE1);
			textureSpecular_.BindTexture();
			// pass projection matrix to shader (note that in this case it could change every frame)
			glm::mat4 projection = glm::perspective(glm::radians(camera_.Zoom),
			                                        (float)width_ / (float)height_, 0.1f, 100.0f);
			shader_.SetMat4("projection", projection);

			// camera/view transformation
			glm::mat4 view = camera_.GetViewMatrix();
			shader_.SetMat4("view", view);


			glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
			glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
			
			shader_.SetInt("light.type", static_cast<int>(lightType_));
			if (lightType_ == LightType::FLASHLIGHT || lightType_ == LightType::SPOTLIGHT)
			{
				shader_.SetVec3("light.position", camera_.Position);
				//std::cout << std::to_string(camera_.Position.x) + " " + std::to_string(camera_.Position.y) + " " + std::to_string(camera_.Position.z) << std::endl;
				shader_.SetVec3("light.direction", camera_.Front);
			} else
			{
				shader_.SetVec3("light.position", lightPosition_);
				//std::cout << std::to_string(lightPosition_.x) + " " + std::to_string(lightPosition_.y) + " " + std::to_string(lightPosition_.z) << std::endl;
				shader_.SetVec3("light.direction", lightDirection_);
			}
			shader_.SetFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
			shader_.SetFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
			shader_.SetFloat("light.constant", 1.0f);
			shader_.SetFloat("light.linear", 0.09f);
			shader_.SetFloat("light.quadratic", 0.032f);
			shader_.SetVec3("light.ambient", ambientColor);
			shader_.SetVec3("light.diffuse", diffuseColor);
			shader_.SetVec3("light.specular", 1.0f, 1.0f, 1.0f);
			
			shader_.SetVec3("material.ambient", 1.0f, 0.5f, 0.31f);
			shader_.SetVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
			shader_.SetVec3("material.specular", 0.5f, 0.5f, 0.5f);
			shader_.SetFloat("material.shininess", 32.0f);
			shader_.SetVec3("viewPos", camera_.Position);

			cube_.BindVao();
			for (unsigned int i = 0; i < 9; i++)
			{
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, cubePositions_[i]);
				float angle = 20.0f * i;
				model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
				shader_.SetMat4("model", model);
				cube_.Render();
			}
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
		const char* items[] = { "DIRECTIONNAL", "POINT", "FLASHLIGHT", "SPOTLIGHT", "NONE"};
		static const char* currentItem = "NONE";

		if (ImGui::BeginCombo("##combo", currentItem))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				bool is_selected = (currentItem == items[n]);
				if (ImGui::Selectable(items[n], is_selected))
				{
					currentItem = items[n];
					lightType_ = static_cast<LightType>(n);
				}
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}

private:
	/*GLuint CreateTexture(char const* Filename)
	{
		gli::texture texture = gli::load_dds(Filename);
		if (texture.empty())
			return 0;

		gli::gl gl(gli::gl::PROFILE_GL33);
		gli::gl::format const format = gl.translate(texture.format(), texture.swizzles());
		GLenum target = gl.translate(texture.target());

		GLuint textureName = 0;
		glGenTextures(1, &textureName);
		glBindTexture(target, textureName);
		glm::tvec3<GLsizei> const extent(texture.extent());
		GLsizei const faceTotal = static_cast<GLsizei>(texture.layers() * texture.faces());

		GLsizei width = extent.x;
		GLsizei height = (texture.target() == gli::TARGET_2D ? extent.y : faceTotal);
		switch (texture.target())
		{
		case gli::TARGET_1D:
			glTexStorage1D(
				target, static_cast<GLint>(texture.levels()), format.Internal, extent.x);
			break;
		case gli::TARGET_1D_ARRAY:
		case gli::TARGET_2D:
		case gli::TARGET_CUBE:
			for (size_t i = 0; i < texture.levels(); i++) {
				glTexImage2D(target, i, format.Internal, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
				width = static_cast<GLsizei>(width / 2.f);
				height = static_cast<GLsizei>(height / 2.f);
			}

		default:
			assert(0);
			break;
		}
		for (std::size_t layer = 0; layer < texture.layers(); ++layer)
			for (std::size_t face = 0; face < texture.faces(); ++face)
				for (std::size_t level = 0; level < texture.levels(); ++level)
				{
					GLsizei const layerGl = static_cast<GLsizei>(layer);
					glm::tvec3<GLsizei> extent(texture.extent(level));
					target = gli::is_target_cube(texture.target())
						? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face)
						: target;

					switch (texture.target())
					{
					case gli::TARGET_1D:
						if (gli::is_compressed(texture.format()))
							glCompressedTexSubImage1D(
								target, static_cast<GLint>(level), 0, extent.x,
								format.Internal, static_cast<GLsizei>(texture.size(level)),
								texture.data(layer, face, level));
						else
							glTexSubImage1D(
								target, static_cast<GLint>(level), 0, extent.x,
								format.External, format.Type,
								texture.data(layer, face, level));
						break;
					case gli::TARGET_1D_ARRAY:
					case gli::TARGET_2D:
					case gli::TARGET_CUBE:
						if (gli::is_compressed(texture.format()))
							glCompressedTexSubImage2D(
								target, static_cast<GLint>(level),
								0, 0,
								extent.x,
								texture.target() == gli::TARGET_1D_ARRAY ? layerGl : extent.y,
								format.Internal, static_cast<GLsizei>(texture.size(level)),
								texture.data(layer, face, level));
						else
							glTexSubImage2D(
								target, static_cast<GLint>(level),
								0, 0,
								extent.x,
								texture.target() == gli::TARGET_1D_ARRAY ? layerGl : extent.y,
								format.External, format.Type,
								texture.data(layer, face, level));
						break;
					case gli::TARGET_2D_ARRAY:
					case gli::TARGET_3D:
					case gli::TARGET_CUBE_ARRAY:
						if (gli::is_compressed(texture.format()))
							glCompressedTexSubImage3D(
								target, static_cast<GLint>(level),
								0, 0, 0,
								extent.x, extent.y,
								texture.target() == gli::TARGET_3D ? extent.z : layerGl,
								format.Internal, static_cast<GLsizei>(texture.size(level)),
								texture.data(layer, face, level));
						else
							glTexSubImage3D(
								target, static_cast<GLint>(level),
								0, 0, 0,
								extent.x, extent.y,
								texture.target() == gli::TARGET_3D ? extent.z : layerGl,
								format.External, format.Type,
								texture.data(layer, face, level));
						break;
					default: assert(0); break;
					}
				}


		return textureName;
	}*/

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
	glm::vec3 cubePositions_[9] = {
		glm::vec3(2.0f, 5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f, 3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f, 2.0f, -2.5f),
		glm::vec3(1.5f, 0.2f, -1.5f),
		glm::vec3(-1.3f, 1.0f, -1.5f)
	};

	enum class LightType
	{
		DIRECTIONNAL = 0,
		POINT = 1,
		FLASHLIGHT = 2,
		SPOTLIGHT = 3,
		NONE = 4
	};

	LightType lightType_ = LightType::NONE;
	
	float timeSince_ = 800.0f;
	
	glm::vec3 lightPosition_ = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 lightDirection_ = glm::vec3(-0.2f, -1.0f, -0.3f);

	Texture textureDiffuse_;
	Texture textureSpecular_;

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
