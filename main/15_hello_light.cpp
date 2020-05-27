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

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "shader.h"
#include "camera.h"



class HelloLightProgram : public RenderProgram
{
public:
	void Init() override
	{
		//texture = CreateTexture("../data/image.dds");

		float vertices[] = {
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
			-0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

			-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
			-0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
			-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

			0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
			0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
			-0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f
		};
		
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO); //size, out bufferID


		glBindVertexArray(VAO);
		// 0. copy our vertices array in a buffer for OpenGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBO); // type, bufferID
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// type, size, data, usage
		// 1. then set the vertex attributes pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		//index(location), size(dim), type, normalize, stride(next point), pointer(offset of the first)
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
		                      (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);


		glGenTextures(1, &texture); //num texture, ID
		glBindTexture(GL_TEXTURE_2D, texture); //type data
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// load and generate the texture
		stbi_set_flip_vertically_on_load(true);
		int width, height, nrChannels;
		unsigned char* data = stbi_load("../data/12_hello_texture/texture.jpg", &width, &height,
		                                &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
			             data);
			//type, level, internalFormat, width, height, border, format, type, data
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}


		stbi_image_free(data);

		shader = Shader("../shaders/14_hello_cube/texture.vert",
		                "../shaders/14_hello_cube/texture.frag");
	}

	void Update(seconds dt) override
	{
		ProcessInput(dt);

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
		shader.setMat4("projection", projection);

		// camera/view transformation
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
		int modelLoc = glGetUniformLocation(shader.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		int viewLoc = glGetUniformLocation(shader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projectionLoc = glGetUniformLocation(shader.ID, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUseProgram(shader.ID);

		glm::vec3 cubePositions[] = {
			glm::vec3(0.0f,  0.0f,  0.0f),
			glm::vec3(2.0f,  5.0f, -15.0f),
			glm::vec3(-1.5f, -2.2f, -2.5f),
			glm::vec3(-3.8f, -2.0f, -12.3f),
			glm::vec3(2.4f, -0.4f, -3.5f),
			glm::vec3(-1.7f,  3.0f, -7.5f),
			glm::vec3(1.3f, -2.0f, -2.5f),
			glm::vec3(1.5f,  2.0f, -2.5f),
			glm::vec3(1.5f,  0.2f, -1.5f),
			glm::vec3(-1.3f,  1.0f, -1.5f)
		};

		glBindVertexArray(VAO);
		for (unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			shader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}

	void Destroy() override
	{
		glDeleteProgram(shader.ID);
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
	}

	void OnEvent(const SDL_Event& event) override
	{
		if (event.type == SDL_MOUSEMOTION)
		{
			if (firstMouse)
			{
				lastX = event.motion.x;
				lastY = event.motion.y;
				firstMouse = false;
			}

			float xoffset = event.motion.x - lastX;
			float yoffset = lastY - event.motion.y; // reversed since y-coordinates go from bottom to top

			lastX = event.motion.x;
			lastY = event.motion.y;

			camera.ProcessMouseMovement(xoffset, yoffset);
		}
		//mouseMotion_ = Vec2f(event.motion.xrel, event.motion.yrel) / mouseMotionRatio_;
		if (event.type == SDL_WINDOWEVENT_RESIZED)
		{
			width = event.window.data1;
			height = event.window.data2;
		}
	}

	void UpdateUi(seconds dt) override
	{
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
			camera.ProcessKeyboard(Camera_Movement::LEFT, dt.count());
		}

		if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
		{
			camera.ProcessKeyboard(Camera_Movement::RIGHT, dt.count());
		}

		if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
		{
			camera.ProcessKeyboard(Camera_Movement::FORWARD, dt.count());
		}

		if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
		{
			camera.ProcessKeyboard(Camera_Movement::BACKWARD, dt.count());
		}

	}
	unsigned VAO = 0;
	unsigned VBO = 0;
	unsigned texture;

	Shader shader;
	
	Camera camera;
	bool firstMouse = true;
	float lastX;
	float lastY;
	float width = 800.0f;
	float height = 600.0f;
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
