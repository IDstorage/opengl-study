#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "shaderprogram.h"
#include "polygon.h"

#include "editorgui.h"

void onResizeCallback(GLFWwindow*, int, int);
void processInput(GLFWwindow*);
void showTriangleColorEditor(ImVec4*, ImVec4*, ImVec4*);
void showBackgroundColorEditor(ImVec4*);

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

	ids::EditorGUI::initialize(window, true);

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

	ImVec4 top_color(1.0f, 0.0f, 0.0f, 1.0f),
		   left_color(0.0f, 1.0f, 0.0f, 1.0f),
		   right_color(0.0f, 0.0f, 1.0f, 1.0f),
		   bg_color(0.0f, 0.0f, 0.0f, 1.0f);
	
#pragma region Rendering Loop
	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		glClearColor(bg_color.x, bg_color.y, bg_color.z, bg_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		// glfwGetTime()으로 현재 시간(sec) 받아옴
		//shader_program->setVec4("customColor", 0.0f, sin(glfwGetTime()) * 0.5f + 0.5f, 0.0f, 1.0f);

		triangle->setVertices({
			0.0f, 0.5f, 0.0f, top_color.x, top_color.y, top_color.z,
			0.5f, -0.5f, 0.0f, right_color.x, right_color.y, right_color.z,
			-0.5f, -0.5f, 0.0f, left_color.x, left_color.y, left_color.z
		});

		triangle->draw();

		glfwPollEvents();

		ids::EditorGUI::startNewFrame();

		showTriangleColorEditor(&top_color, &left_color, &right_color);
		showBackgroundColorEditor(&bg_color);

		ids::EditorGUI::render();

		glfwSwapBuffers(window);
	}
#pragma endregion

	ids::EditorGUI::release();

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

void showTriangleColorEditor(ImVec4* top_color, ImVec4* left_color, ImVec4* right_color) {
	ImGui::Begin("Triangle Color", (bool*)false, ImGuiWindowFlags_NoResize);
	ImGui::SetWindowSize(ImVec2(350.0f, 100.0f));

	ImGui::ColorEdit3("Top color", reinterpret_cast<float*>(top_color));
	ImGui::ColorEdit3("Left color", reinterpret_cast<float*>(left_color));
	ImGui::ColorEdit3("Right color", reinterpret_cast<float*>(right_color));

	ImGui::End();
}

void showBackgroundColorEditor(ImVec4* bg_color) {
	ImGui::Begin("Background Color", (bool*)false, ImGuiWindowFlags_NoResize);
	ImGui::SetWindowSize(ImVec2(350.0f, 55.0f));

	ImGui::ColorEdit3("Background", reinterpret_cast<float*>(bg_color));

	ImGui::End();
}