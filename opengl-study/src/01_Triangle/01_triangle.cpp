#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "shaderprogram.h"
#include "polygon.h"

void onResizeCallback(GLFWwindow*, int, int);
void processInput(GLFWwindow*);

bool is_wfkey_pressed = false;

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
	glfwSetFramebufferSizeCallback(window, onResizeCallback);

#pragma region Shader Compile/Link
	auto shader_program = std::make_shared<ids::ShaderProgram>();
	bool ret = shader_program->attach(GL_VERTEX_SHADER, "./src/01_Triangle/01_triangle.vert");
	ret &= shader_program->attach(GL_FRAGMENT_SHADER, "./src/01_Triangle/01_triangle.frag");
	if (!ret) {
		glfwTerminate();
		return -1;
	}
	
	ret = shader_program->link();
	if (!ret) {
		glfwTerminate();
		return -1;
	}

	auto shader_program2 = std::make_shared<ids::ShaderProgram>();
	ret = shader_program2->attach(GL_VERTEX_SHADER, "./src/01_Triangle/01_triangle.vert");
	ret &= shader_program2->attach(GL_FRAGMENT_SHADER, "./src/01_Triangle/01_triangle_2.frag");
	if (!ret) {
		glfwTerminate();
		return -1;
	}

	ret = shader_program2->link();
	if (!ret) {
		glfwTerminate();
		return -1;
	}
#pragma endregion

	/* 
	 * NDC(Normalized Device Coordinates)
	 *  - ��� �࿡ ���� ���� ������ [-1, 1]�� ���ѵǸ� ��� ���� ������ ���� �ʴ´�.
	 *  - ���ϴ� (-1, -1) / ���� (1, 1)
	 * 
	 * �� NDC�� glViewport �Լ��� ������ ������(�ȼ� ��)�� �������� Screen-space coordinates(ȭ�� ��ǥ)�� ��ȯ�ȴ�. (Viewport transform, ����Ʈ ��ȯ)
	 * �׷��� ��ȯ�� screen-space coordinates�� fragment�� ��ȯ�Ǿ� fragment shader�� �Է����� ���޵ȴ�.
	 */
	auto left_triangle = std::make_unique<ids::Polygon>();
	left_triangle->setVertices({
		-0.525f, 0.525f, 0.0f,	// left-top
		-0.525f, -0.475f, 0.0f,	// left-bottom
		0.475f, 0.525f, 0.0f		// right-top
	});
	left_triangle->setIndices({
		//0, 1, 2		// Counter-Clockwise
		0, 2, 1	// Clockwise
	});
	left_triangle->setTargetShaderProg(shader_program);

	auto right_triangle = std::make_unique<ids::Polygon>();
	right_triangle->setVertices({
		0.525f, 0.475f, 0.0f,	// right-top
		-0.475f, -0.525f, 0.0f,	// left-bottom
		0.525f, -0.525f, 0.0f		// right-bottom
	});
	right_triangle->setIndices({
		0, 1, 2		// Counter-Clockwise
		//0, 2, 1	// Clockwise
	});
	right_triangle->setWireframeMode(true);
	right_triangle->setTargetShaderProg(shader_program2);

#pragma region Rendering Loop
	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		left_triangle->setWireframeMode(is_wfkey_pressed);
		left_triangle->draw();

		right_triangle->setWireframeMode(!is_wfkey_pressed);
		right_triangle->draw();

		glfwPollEvents();
		glfwSwapBuffers(window);
	}
#pragma endregion

	shader_program.reset();
	shader_program2.reset();

	left_triangle.reset();
	right_triangle.reset();

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

	is_wfkey_pressed = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
}