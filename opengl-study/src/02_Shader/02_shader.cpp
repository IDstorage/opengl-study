#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "shaderprogram.h"
#include "polygon.h"

void onResizeCallback(GLFWwindow*, int, int);
void processInput(GLFWwindow*);

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "02 Shader", nullptr, nullptr);
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
	glfwSetFramebufferSizeCallback(window, onResizeCallback);

#pragma region Shader Compile/Link
	auto shader_program = std::make_shared<ids::ShaderProgram>();
	
	bool ret = true;
	/*{
		ret &= shaderProgram->Attach(GL_VERTEX_SHADER, "./src/02_Shader/02_shader.vert");
		ret &= shaderProgram->Attach(GL_FRAGMENT_SHADER, "./src/02_Shader/02_shader.frag");
	}
	{
		ret &= shaderProgram->Attach(GL_FRAGMENT_SHADER, "./src/02_Shader/02_shader_2.frag");
	}*/
	{
		ret &= shader_program->attach(GL_VERTEX_SHADER, "./src/02_Shader/02_shader_3.vert");
		ret &= shader_program->attach(GL_FRAGMENT_SHADER, "./src/02_Shader/02_shader_3.frag");
	}

	if (!ret) {
		glfwTerminate();
		return -1;
	}

	ret = shader_program->link();
	if (!ret) {
		glfwTerminate();
		return -1;
	}
#pragma endregion


#pragma region Create polygon
	auto triangle = std::make_unique<ids::Polygon>();
	//triangle->SetVertices({
	//	0.0f, 0.5f, 0.0f,
	//	0.5f, -0.5f, 0.0f,
	//	-0.5f, -0.5f, 0.0f
	//});
	triangle->setVertices({
		0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f
	});
	triangle->setIndices({
		0, 2, 1 // Counter-clockwise
	});

	triangle->setAttributes(0, 3, GL_FLOAT, false, 6 * sizeof(float), NULL);
	triangle->setAttributes(1, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	
	triangle->setWireframeMode(false);
	triangle->setTargetShaderProg(shader_program);
#pragma endregion

	
#pragma region Rendering Loop
	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// glfwGetTime()으로 현재 시간(sec) 받아옴
		//shader_program->setVec4("customColor", 0.0f, sin(glfwGetTime()) * 0.5f + 0.5f, 0.0f, 1.0f);

		triangle->draw();

		glfwPollEvents();
		glfwSwapBuffers(window);
	}
#pragma endregion

	shader_program.reset();
	triangle.reset();

	glfwTerminate();
	return 0;
}

void onResizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}