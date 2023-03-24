#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "shaderprogram.h"
#include "polygon.h"

#include "editorgui.h"

void onResizeCallback(GLFWwindow*, int, int);
void processInput(GLFWwindow*);
void showTriangleOptionEditor(bool&, bool&);

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

	ids::EditorGUI::initialize(window, true);

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
	 *  - 모든 축에 대해 값의 범위가 [-1, 1]로 국한되며 벗어난 값은 렌더링 되지 않는다.
	 *  - 좌하단 (-1, -1) / 우상단 (1, 1)
	 * 
	 * 이 NDC는 glViewport 함수에 전달한 데이터(픽셀 수)를 바탕으로 Screen-space coordinates(화면 좌표)로 변환된다. (Viewport transform, 뷰포트 변환)
	 * 그렇게 변환된 screen-space coordinates는 fragment로 변환되어 fragment shader의 입력으로 전달된다.
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
	left_triangle->setAttributes(0, 3, GL_FLOAT, false, 3 * sizeof(float), NULL);
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
	right_triangle->setAttributes(0, 3, GL_FLOAT, false, 3 * sizeof(float), NULL);
	right_triangle->setWireframeMode(true);
	right_triangle->setTargetShaderProg(shader_program2);

	bool left_triangle_wf = false, right_triangle_wf = true;

#pragma region Rendering Loop
	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		left_triangle->setWireframeMode(left_triangle_wf);
		left_triangle->draw();

		right_triangle->setWireframeMode(right_triangle_wf);
		right_triangle->draw();

		glfwPollEvents();

		ids::EditorGUI::startNewFrame();

		showTriangleOptionEditor(left_triangle_wf, right_triangle_wf);

		ids::EditorGUI::render();

		glfwSwapBuffers(window);
	}
#pragma endregion

	ids::EditorGUI::release();

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
}

void showTriangleOptionEditor(bool& left, bool& right) {
	ImGui::Begin("Setting", (bool*)false, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	ImGui::SetWindowSize(ImVec2(300.0f, 125.0f));
	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));

	if (ImGui::CollapsingHeader("Left Triangle", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Checkbox("Active wire-frame mode##left", &left);
	}

	if (ImGui::CollapsingHeader("Right Triangle", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Checkbox("Active wire-frame mode##right", &right);
	}

	ImGui::End();
}