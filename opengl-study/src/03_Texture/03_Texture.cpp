#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "CustomShaderProgram.h"
#include "Polygon.h"
#include "Texture.h"

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600

void OnResizeCallback(GLFWwindow*, int, int);
void ProcessInput(GLFWwindow*);

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "03 Texture", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to intialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glfwSetFramebufferSizeCallback(window, OnResizeCallback);

#pragma region Shader Compile/Link
	auto shaderProgram = std::make_shared<CustomShaderProgram>();

	bool ret = true;
	{
		ret &= shaderProgram->Attach(GL_VERTEX_SHADER, "./src/03_Texture/03_Texture.vert");
		ret &= shaderProgram->Attach(GL_FRAGMENT_SHADER, "./src/03_Texture/03_Texture.frag");
	}

	if (!ret) {
		glfwTerminate();
		return -1;
	}

	ret = shaderProgram->Link();
	if (!ret) {
		glfwTerminate();
		return -1;
	}
#pragma endregion


#pragma region Create polygon
	auto box = std::make_unique<Polygon>();
	box->SetVertices({
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
		});
	box->SetIndices({
		2, 0, 3,
		2, 1, 0
		});

	box->SetAttributes(0, 3, GL_FLOAT, false, 8 * sizeof(float), NULL);
	box->SetAttributes(1, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	box->SetAttributes(2, 2, GL_FLOAT, false, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	box->SetTargetShaderProg(shaderProgram);

	auto texture = std::make_shared<Texture>("./resources/uv_checker.jpg");
	texture->SetWrapOption(GL_REPEAT, GL_REPEAT);
	texture->SetMinifyFilter(GL_LINEAR);
	texture->SetMagnifyFilter(GL_LINEAR);

	box->SetTargetTexture(texture);
#pragma endregion


#pragma region Rendering Loop
	while (!glfwWindowShouldClose(window)) {
		ProcessInput(window);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		box->Draw();

		glfwPollEvents();
		glfwSwapBuffers(window);
	}
#pragma endregion

	shaderProgram.reset();                                          
	box.reset();
	texture.reset();

	glfwTerminate();
	return 0;
}

void OnResizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}