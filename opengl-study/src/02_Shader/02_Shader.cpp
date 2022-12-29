#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Common/CustomShaderProgram.h"
#include "Common/Polygon.h"

void OnResizeCallback(GLFWwindow*, int, int);
void ProcessInput(GLFWwindow*);

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "01 Triangle", nullptr, nullptr);
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

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, OnResizeCallback);

#pragma region Shader Compile/Link
	auto shaderProgram = std::make_unique<CustomShaderProgram>();
	bool ret = shaderProgram->Attach(GL_VERTEX_SHADER, "./src/02_Shader/02_Shader.vert");
	ret &= shaderProgram->Attach(GL_FRAGMENT_SHADER, "./src/02_Shader/02_Shader.frag");
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
	auto triangle = std::make_unique<Polygon>();
	triangle->SetVertices({
		0.0f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f
	});
	triangle->SetIndices({
		0, 2, 1 // Counter-clockwise
	});
	triangle->SetWireframeMode(false);
#pragma endregion

	
#pragma region Rendering Loop
	while (!glfwWindowShouldClose(window)) {
		ProcessInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shaderProgram->Use();
		triangle->Draw();

		glfwPollEvents();
		glfwSwapBuffers(window);
	}
#pragma endregion

	shaderProgram.reset();

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