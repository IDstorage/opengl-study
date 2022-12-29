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
#include <ctime>
#include <random>
#include <functional>

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
	
	// ������ ����
	GLFWwindow* window = glfwCreateWindow(800, 600, "00 Window", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// window�� ���ؽ�Ʈ�� �� ���ؽ�Ʈ�� ����
	glfwMakeContextCurrent(window);

	// GLAD: OpenGL�� �Լ� ������ ����
	// ������ �� OS ȯ�濡 ���� �Լ��� �����ϴ� glfwGetProcAddress �Լ��� ����
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to intialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);
	// â ������ ���� �ݹ�
	glfwSetFramebufferSizeCallback(window, OnResizeCallback);

	std::mt19937 randomEngine{ static_cast<unsigned int>(time(0)) };
	std::uniform_real_distribution<> distribution(0.0, 1.0);

	double r = distribution(randomEngine);
	double g = distribution(randomEngine);
	double b = distribution(randomEngine);

	// ���� ����
	while (!glfwWindowShouldClose(window)) { // �����ؾ� �ϴ��� Ȯ��
		// �Է� ó��
		ProcessInput(window);
		
		// ������ ó��

		// glClear�� �ʱ�ȭ�� ���� �� �÷� ���۸� � ������ �ʱ�ȭ���� ����
		// ���� ���ؼ� glClear�� ȣ��Ǳ� ���� �ƹ� �ϵ� �Ͼ�� �ʴ´�.
		glClearColor(r, g, b, 1.0f);
		// glClearXXX�� ������ ������ ���۸� ����. (��Ȯ���� ������)
		// GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT(���� ����), GL_STENCIL_BUFFER_BIT(���ٽ� ����)�� �ִ�.
		glClear(GL_COLOR_BUFFER_BIT);
		//glClear(GL_DEPTH_BUFFER_BIT);
		//glClear(GL_STENCIL_BUFFER_BIT);

		// �̺�Ʈ(Ű���� �Է��̳� ���콺 �̵� ��)�� �߻��ߴ��� Ȯ�� �� ��ϵ� �ݹ� �Լ����� ȣ��
		glfwPollEvents();
		// �÷� ���� ��ü (������۸�)
		glfwSwapBuffers(window);
	}

	// ��� ���ҽ� ����
	glfwTerminate();
	return 0;
}

// â ����� ����Ǹ� ȣ��
void OnResizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window) {
	// Ű �Է� (DirectX�� ����ѵ�?)
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		// ������ ���� �÷��� ���� (�� bool���� ���� ������ glfwWindowShouldClose(...)�� Ȯ�� ����) 
		glfwSetWindowShouldClose(window, true);
	}
}