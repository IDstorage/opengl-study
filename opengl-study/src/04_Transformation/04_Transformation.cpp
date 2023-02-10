#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "glm_required.h"

#include "shaderprogram.h"
#include "polygon.h"
#include "texture.h"
#include "transform.h"

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600

void OnResizeCallback(GLFWwindow*, int, int);
void ProcessInput(GLFWwindow*);

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "04 Transformation", nullptr, nullptr);
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

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glfwSetFramebufferSizeCallback(window, OnResizeCallback);

#pragma region Shader Compile/Link
	auto shader_program = std::make_shared<ids::ShaderProgram>();

	bool ret = true;
	{
		ret &= shader_program->attach(GL_VERTEX_SHADER, "./src/04_Transformation/04_transformation.vert");
		ret &= shader_program->attach(GL_FRAGMENT_SHADER, "./src/04_Transformation/04_transformation.frag");
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
	auto box = std::make_unique<ids::Polygon>();
	box->setVertices({
		0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f, 0.0f, 1.0f
		});
	box->setIndices({
		2, 0, 3,
		2, 1, 0
		});

	box->setAttributes(0, 3, GL_FLOAT, false, 5 * sizeof(float), NULL);
	box->setAttributes(1, 2, GL_FLOAT, false, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	box->setTargetShaderProg(shader_program);

	auto texture = std::make_shared<ids::Texture>("./resources/uv_checker.jpg");
	texture->setIndex(GL_TEXTURE0);
	texture->setWrapOption(GL_REPEAT, GL_REPEAT);
	texture->setMinifyFilter(GL_LINEAR);
	texture->setMagnifyFilter(GL_LINEAR);

	auto overlay_texture = std::make_shared<ids::Texture>("./resources/cpplogo.png");
	overlay_texture->setIndex(GL_TEXTURE1);
	overlay_texture->setWrapOption(GL_REPEAT, GL_REPEAT);
	overlay_texture->setMinifyFilter(GL_LINEAR);
	overlay_texture->setMagnifyFilter(GL_LINEAR);

	shader_program->setInt("texture1", 0);
	shader_program->setInt("texture2", 1);

	box->addTargetTexture(texture);
	box->addTargetTexture(overlay_texture);
#pragma endregion


#pragma region GLM
	// glm 0.9.9 �̻���� ����� ���� ����� �ƴ� 0���� �ʱ�ȭ�ȴ�.
	// ���� ����� ���ؼ��� �����ڿ� 1.0�� �����Ѵ�.
	glm::mat4 tr1(1.0f);
	// �̵����
	tr1 = glm::translate(tr1, glm::vec3(1.0f, 1.0f, 0.0f));
	/*
	 * ȸ�����
	 * ���ڷ� radian ���� ���� ����. ���� ����ȭ�Ǿ� �־�� �Ѵ�.
	 */
	tr1 = glm::rotate(tr1, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	tr1 = glm::scale(tr1, glm::vec3(0.5f, 0.5f, 0.5f));
	
	glm::vec4 v1(1.0, 0.0, 0.0, 1.0);
	// OpenGL�� ������ ��ǥ���̸� �����͸� ����ϱ� ������ TRS�̴�. (T*R*S*v)
	v1 = tr1 * v1;

	glm::mat4 tr2(1.0f);
	//tr2 = glm::translate(tr2, glm::vec3(0.0f, 0.5f, 0.0f));
	//tr2 = glm::rotate(tr2, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//tr2 = glm::scale(tr2, glm::vec3(0.5f, 1.0f, 0.5f));

	shader_program->setMat4("transform", tr2);

	ids::Transform transform;
#pragma endregion


#pragma region Rendering Loop
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (!glfwWindowShouldClose(window)) {
		ProcessInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader_program->setFloat("threshold", sin(glfwGetTime()) * 0.5f + 0.5f);

		transform.rotateTo(0.0f, 0.0f, static_cast<float>(glfwGetTime()) * 360.0f);
		transform.scaleTo(glm::vec3(sin(glfwGetTime()) * 0.5f + 0.5f, cos(glfwGetTime()) * 0.5f + 0.5f, 1.0f));
		shader_program->setMat4("transform", transform.getTransform());

		box->draw();

		glfwPollEvents();
		glfwSwapBuffers(window);
	}
#pragma endregion

	glDisable(GL_BLEND);

	shader_program.reset();
	box.reset();
	texture.reset();
	overlay_texture.reset();

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