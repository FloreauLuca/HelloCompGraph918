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
#include <easy/profiler.h>

#include "shader.h"
#include "camera.h"
#include "cube.h"
#include "model.h"


class HelloPointShadowsProgram : public RenderProgram
{
public:
	unsigned InitQuad() {

		glm::vec2 translations[100];
		int index = 0;
		const float offset = 0.1f;
		for (int y = -10; y < 10; y += 2)
		{
			for (int x = -10; x < 10; x += 2)
			{
				glm::vec2 translation;
				translation.x = (float)x / 10.0f + offset;
				translation.y = (float)y / 10.0f + offset;
				translations[index++] = translation;
			}
		}
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		unsigned int VAO;
		glGenVertexArrays(1, &VAO);

		unsigned int VBO;
		glGenBuffers(1, &VBO); //size, out bufferID

		// 1. bind Vertex Array Object
		glBindVertexArray(VAO);
		// 2. copy our vertices array in a vertex buffer for OpenGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBO); // type, bufferID
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices_), quadVertices_, GL_STATIC_DRAW);// type, size, data, usage
		// 4. then set the vertex attributes pointers
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); //index(location), size(dim), type, normalize, stride(next point), pointer(offset of the first)
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float))); //index(location), size(dim), type, normalize, stride(next point), pointer(offset of the first)
		glEnableVertexAttribArray(1);

		unsigned int instanceVBO;
		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glVertexAttribDivisor(2, 1);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		return VAO;
	}

	void Init() override
	{
		EASY_BLOCK("Init");
		//texture = CreateTexture("../data/image.dds");
		cube_.Init();
		shader_ = Shader("../shaders/111_hello_instancing/model.vert",
			"../shaders/111_hello_instancing/model.frag");

		shaderQuad_ = Shader("../shaders/111_hello_instancing/quad.vert",
			"../shaders/111_hello_instancing/quad.frag");
		
		model_.loadModel("../data/111_hello_instancing/enemy.obj");
		quadVAO_ = InitQuad();
		
		modelMatrices_ = new glm::mat4[maxAmount];
		srand(glfwGetTime()); // initialize random seed	
		float radius = 10.0f;
		float offset = 8.0f;
		for (unsigned int i = 0; i < maxAmount; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			// 1. translation: displace along circle with 'radius' in range [-offset, offset]
			float angle = (float)i / (float)maxAmount * 360.0f;
			float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float x = sin(angle) * radius + displacement;
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
			displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			float z = cos(angle) * radius + displacement;
			model = glm::translate(model, glm::vec3(x, 0, z));

			// 2. scale: scale between 0.05 and 0.25f
			float scale = 0.01f;
			model = glm::scale(model, glm::vec3(scale));

			// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
			//float rotAngle = (rand() % 360);
			//model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

			// 4. now add to list of matrices
			modelMatrices_[i] = model;
		}
		// vertex buffer object
		unsigned int buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, maxAmount * sizeof(glm::mat4), &modelMatrices_[0], GL_STATIC_DRAW);

		for (unsigned int i = 0; i < model_.meshes.size(); i++)
		{
			unsigned int VAO = model_.meshes[i].VAO;
			glBindVertexArray(VAO);
			// vertex attributes
			std::size_t vec4Size = sizeof(glm::vec4);
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
			glEnableVertexAttribArray(5);
			glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
			glEnableVertexAttribArray(6);
			glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

			glVertexAttribDivisor(3, 1);
			glVertexAttribDivisor(4, 1);
			glVertexAttribDivisor(5, 1);
			glVertexAttribDivisor(6, 1);

			glBindVertexArray(0);
		}
	}

	void Update(seconds dt) override
	{
		EASY_BLOCK("Input");
		timeSince_ += dt.count();
		ProcessInput(dt);
		EASY_END_BLOCK;
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		EASY_BLOCK("Draw");
		// don't forget to enable shader before setting uniforms
		shader_.Use();

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera_.Zoom), (float)width_ / (float)height_, 0.1f, 100.0f);
		glm::mat4 view = camera_.GetViewMatrix();
		shader_.SetMat4("projection", projection);
		shader_.SetMat4("view", view);

		/*
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		shader_.SetMat4("model", model);
		model_.Draw(shader_);*/

		// draw meteorites
		shader_.Use();
		for (unsigned int i = 0; i < model_.meshes.size(); i++)
		{
			glBindVertexArray(model_.meshes[i].VAO);
			glDrawElementsInstanced(
				GL_TRIANGLES, model_.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount
			);
		}


	}

	void Destroy() override
	{
		shader_.Destroy();
		cube_.Destroy();
		glDeleteVertexArrays(1, &quadVAO_);
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
		ImGui::SliderInt("Number", &amount, 1, maxAmount);
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

	int amount = 1000;
	int maxAmount = 10000;
	glm::mat4* modelMatrices_;
	glm::vec3 modelPosition_ = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cubePosition_[4] =
	{
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f)
	};

	float quadVertices_[30] = {
		// positions     // colors
		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		-0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		 0.05f,  0.05f,  0.0f, 1.0f, 1.0f
	};
	
	Shader shader_;
	Shader shaderQuad_;
	unsigned quadVAO_;

	RenderCube cube_;

	Camera camera_;
	bool firstMouse_ = true;
	bool rightMouse_ = false;
	float lastX;
	float lastY;
	float width_ = 800.0f;
	float height_ = 600.0f;

	Model model_;
};

int main(int argc, char** argv)
{
	EASY_PROFILER_ENABLE;
	Engine engine;
	HelloPointShadowsProgram renderProgram;
	engine.AddProgram(&renderProgram);
	engine.Init();
	engine.StartLoop();
	profiler::dumpBlocksToFile("../profile/111_hello_instancing.prof");
	return EXIT_SUCCESS;
}
