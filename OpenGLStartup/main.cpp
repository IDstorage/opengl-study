/* OpenGL�� ���� ������ ����̹��� �����ϱ� ������ ��κ��� �Լ� ��ġ���� ������ �ÿ� �� �� ���� ��Ÿ�� ��
 * �Ʒ��� ���� �����ؾ� �Ѵ�. �׸��� �̸� ���߿� ����� �� �ֵ��� �Լ� �����ͷ� �����Ѵ�.
 */

//typedef void (*GL_GENBUFFERS) (GLsizei, GLuint*);
//GL_GENBUFFERS glGenBuffers = (GL_GENBUFFERS)glfwGetProcAddress("glGenBuffers");
//unsigned int buffer;
//glGenBuffers(1, &buffer);

/* �̸� �����ϱ� ���� GLAD�� �ʿ� (����̹��� �°� �Լ��� �ҷ���)
 * ������ �� ���Ѿ� �Ѵ�!! (glad -> glfw3)
 */
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>

void OnResizeCallback(GLFWwindow*, int, int);
void ProcessInput(GLFWwindow*);

int main() {
	// GLFW �ʱ�ȭ
	glfwInit();

	/*
	 * glfwWindowHint()
	 *  - GLFW �����ϴ� �Լ�
	 *		p1. �����ϰ��� �ϴ� �ɼ� (enum, GLFW_ ���λ�)
	 *		p2. �� (����)
	 * *��� �ɼǰ� ��: https://www.glfw.org/docs/latest/window.html#window_hints
	 */

	// ����� OpenGL ������ GLFW���� �˷��ش�
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Mac OS X�� �ش� �ڵ� �߰�
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// ����� OpenGL ������(����� Core)�� GLFW���� �˷��ش�
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Startup", nullptr, nullptr);
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

	while (!glfwWindowShouldClose(window)) {
		// �Է� ó��
		ProcessInput(window);
		
		// ������
		glClearColor(0.81f, 1.0f, 0.89f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// �̺�Ʈ Ȯ���ϰ� ���� ��ü
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

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