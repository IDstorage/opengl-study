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
#include <ctime>
#include <random>
#include <functional>

#include "editorgui.h"

void onResizeCallback(GLFWwindow*, int, int);
void processInput(GLFWwindow*);
void showBackgroundColorEditor(ImVec4*);

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
	
	// 윈도우 생성
	GLFWwindow* window = glfwCreateWindow(800, 600, "00 Window", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// window의 컨텍스트를 주 컨텍스트로 지정
	glfwMakeContextCurrent(window);

	// GLAD: OpenGL용 함수 포인터 관리
	// 컴파일 할 OS 환경에 따라 함수를 정의하는 glfwGetProcAddress 함수를 전달
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to intialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);
	// 창 사이즈 변경 콜백
	glfwSetFramebufferSizeCallback(window, onResizeCallback);

	ImVec4 bg_color(0.0f, 0.0f, 0.0f, 1.0f);

	ids::EditorGUI::initialize(window, "#version 130");

	// 메인 루프
	while (!glfwWindowShouldClose(window)) { // 종료해야 하는지 확인
		// 입력 처리
		processInput(window);
		
		// 렌더링 처리

		// glClear로 초기화할 버퍼 중 컬러 버퍼를 어떤 색으로 초기화할지 결정
		// 색상만 정해서 glClear가 호출되기 전엔 아무 일도 일어나지 않는다.
		glClearColor(bg_color.x, bg_color.y, bg_color.z, bg_color.w);
		// glClearXXX로 설정한 값으로 버퍼를 비운다. (정확히는 덮어씌운다)
		// GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT(깊이 버퍼), GL_STENCIL_BUFFER_BIT(스텐실 버퍼)가 있다.
		glClear(GL_COLOR_BUFFER_BIT);
		//glClear(GL_DEPTH_BUFFER_BIT);
		//glClear(GL_STENCIL_BUFFER_BIT);

		// 이벤트(키보드 입력이나 마우스 이동 등)가 발생했는지 확인 후 등록된 콜백 함수들을 호출
		glfwPollEvents();

		ids::EditorGUI::startNewFrame();

		showBackgroundColorEditor(&bg_color);

		ids::EditorGUI::render();

		// 컬러 버퍼 교체 (더블버퍼링)
		glfwSwapBuffers(window);
	}

	ids::EditorGUI::release();

	// 모든 리소스 해제
	glfwTerminate();
	return 0;
}

// 창 사이즈가 변경되면 호출
void onResizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	// 키 입력 (DirectX랑 비슷한듯?)
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		// 윈도우 종료 플래그 설정 (이 bool값은 메인 루프의 glfwWindowShouldClose(...)로 확인 가능) 
		glfwSetWindowShouldClose(window, true);
	}
}

void showBackgroundColorEditor(ImVec4* bg_color) {
	ImGui::Begin("Color editor", (bool*)false, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	ImGui::SetWindowSize(ImVec2(400.0f, 65.0f));
	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));

	ImGui::ColorEdit3("Background Color", reinterpret_cast<float*>(bg_color));

	ImGui::End();
}