#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Common/CustomShaderProgram.h"
#include "Common/Polygon.h"

void OnResizeCallback(GLFWwindow*, int, int);
void ProcessInput(GLFWwindow*);

bool isWFKeyPressed = false;

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
	bool ret = shaderProgram->Attach(GL_VERTEX_SHADER, "./src/01_Triangle/01_Triangle.vert");
	ret &= shaderProgram->Attach(GL_FRAGMENT_SHADER, "./src/01_Triangle/01_Triangle.frag");
	if (!ret) {
		glfwTerminate();
		return -1;
	}
	
	ret = shaderProgram->Link();
	if (!ret) {
		glfwTerminate();
		return -1;
	}

	auto shaderProgram2 = std::make_unique<CustomShaderProgram>();
	ret = shaderProgram2->Attach(GL_VERTEX_SHADER, "./src/01_Triangle/01_Triangle.vert");
	ret &= shaderProgram2->Attach(GL_FRAGMENT_SHADER, "./src/01_Triangle/01_Triangle_2.frag");
	if (!ret) {
		glfwTerminate();
		return -1;
	}

	ret = shaderProgram2->Link();
	if (!ret) {
		glfwTerminate();
		return -1;
	}
#pragma endregion

	/* 
	 * NDC(Normalized Device Coordinates)
	 *  - 모든 축에 대해 값의 범위가 [-1, 1]로 국한되며 벗어난 값은 렌더링 되지 않는다.
	 *  - 좌하단 (-1, -1) / 우상단 (1, 1)
	 * 
	 * 이 NDC는 glViewport 함수에 전달한 데이터(픽셀 수)를 바탕으로 Screen-space coordinates(화면 좌표)로 변환된다. (Viewport transform, 뷰포트 변환)
	 * 그렇게 변환된 screen-space coordinates는 fragment로 변환되어 fragment shader의 입력으로 전달된다.
	 */
	auto leftTriangle = std::make_unique<Polygon>();
	leftTriangle->SetVertices({
		-0.525f, 0.525f, 0.0f,	// left-top
		-0.525f, -0.475f, 0.0f,	// left-bottom
		0.475f, 0.525f, 0.0f		// right-top
	});
	leftTriangle->SetIndices({
		//0, 1, 2		// Counter-Clockwise
		0, 2, 1	// Clockwise
	});

	auto rightTriangle = std::make_unique<Polygon>();
	rightTriangle->SetVertices({
		0.525f, 0.475f, 0.0f,	// right-top
		-0.475f, -0.525f, 0.0f,	// left-bottom
		0.525f, -0.525f, 0.0f		// right-bottom
	});
	rightTriangle->SetIndices({
		0, 1, 2		// Counter-Clockwise
		//0, 2, 1	// Clockwise
	});
	rightTriangle->SetWireframeMode(true);

#pragma region Rendering Loop
	while (!glfwWindowShouldClose(window)) {
		ProcessInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shaderProgram->Use();
		leftTriangle->SetWireframeMode(isWFKeyPressed);
		leftTriangle->Draw();

		shaderProgram2->Use();
		rightTriangle->SetWireframeMode(!isWFKeyPressed);
		rightTriangle->Draw();

		glfwPollEvents();
		glfwSwapBuffers(window);
	}
#pragma endregion

	shaderProgram.reset();
	shaderProgram2.reset();

	leftTriangle.reset();
	rightTriangle.reset();

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

	isWFKeyPressed = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
}