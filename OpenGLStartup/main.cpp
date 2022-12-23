/* OpenGL은 많은 버전의 드라이버가 존재하기 때문에 대부분의 함수 위치들은 컴파일 시에 알 수 없어 런타임 시
 * 아래와 같이 질의해야 한다. 그리고 이를 나중에 사용할 수 있도록 함수 포인터로 저장한다.
 */

//typedef void (*GL_GENBUFFERS) (GLsizei, GLuint*);
//GL_GENBUFFERS glGenBuffers = (GL_GENBUFFERS)glfwGetProcAddress("glGenBuffers");
//unsigned int buffer;
//glGenBuffers(1, &buffer);

/* 이를 방지하기 위해 GLAD가 필요 (드라이버에 맞게 함수를 불러옴)
 * 순서를 꼭 지켜야 한다!! (glad -> glfw3)
 */
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>

void OnResizeCallback(GLFWwindow*, int, int);
void ProcessInput(GLFWwindow*);

int main() {
	// GLFW 초기화
	glfwInit();

	/*
	 * glfwWindowHint()
	 *  - GLFW 설정하는 함수
	 *		p1. 설정하고자 하는 옵션 (enum, GLFW_ 접두사)
	 *		p2. 값 (정수)
	 * *모든 옵션과 값: https://www.glfw.org/docs/latest/window.html#window_hints
	 */

	// 사용할 OpenGL 버전을 GLFW에게 알려준다
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Mac OS X는 해당 코드 추가
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// 사용할 OpenGL 프로필(현재는 Core)을 GLFW에게 알려준다
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
		// 입력 처리
		ProcessInput(window);
		
		// 렌더링
		glClearColor(0.81f, 1.0f, 0.89f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// 이벤트 확인하고 버퍼 교체
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