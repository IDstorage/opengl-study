#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "CustomShaderProgram.h"
#include "Polygon.h"
#include "Texture.h"

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600

void OnResizeCallback(GLFWwindow*, int, int);
void ProcessInput(GLFWwindow*);

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "03 Texture", nullptr, nullptr);
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
	auto shaderProgram = std::make_shared<CustomShaderProgram>();

	bool ret = true;
	{
		ret &= shaderProgram->Attach(GL_VERTEX_SHADER, "./src/03_Texture/03_Texture.vert");
		ret &= shaderProgram->Attach(GL_FRAGMENT_SHADER, "./src/03_Texture/03_Texture_2.frag");
	}

	if (!ret) {
		glfwTerminate();
		return -1;
	}

	ret = shaderProgram->Link();
	if (!ret) {
		glfwTerminate();
		return -1;
	}
#pragma endregion


#pragma region Create polygon
	auto box = std::make_unique<Polygon>();
	box->SetVertices({
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
		});
	box->SetIndices({
		2, 0, 3,
		2, 1, 0
		});

	box->SetAttributes(0, 3, GL_FLOAT, false, 8 * sizeof(float), NULL);
	box->SetAttributes(1, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	box->SetAttributes(2, 2, GL_FLOAT, false, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	box->SetTargetShaderProg(shaderProgram);

	auto texture = std::make_shared<Texture>("./resources/uv_checker.jpg");
	texture->SetIndex(GL_TEXTURE0);
	texture->SetWrapOption(GL_REPEAT, GL_REPEAT);
	texture->SetMinifyFilter(GL_LINEAR);
	texture->SetMagnifyFilter(GL_LINEAR);

	auto overlayTexture = std::make_shared<Texture>("./resources/cpplogo.png");
	overlayTexture->SetIndex(GL_TEXTURE1);
	overlayTexture->SetWrapOption(GL_REPEAT, GL_REPEAT);
	overlayTexture->SetMinifyFilter(GL_LINEAR);
	overlayTexture->SetMagnifyFilter(GL_LINEAR);

	shaderProgram->SetInt("texture1", 0);
	shaderProgram->SetInt("texture2", 1);

	box->AddTargetTexture(texture);
	box->AddTargetTexture(overlayTexture);
#pragma endregion


#pragma region Rendering Loop
	/*
	 * GL_BLEND(����) ����� ����Ѵ�. glDisable(...)�� �Բ� ����Ѵ�.
	 * 
	 * �ɼ����δ�
	 *   - GL_BLEND : ���� ����
	 *   - GL_CULL_FACE : ������ �߷�����
	 *   - GL_DEPTH_TEST : ���� �׽�Ʈ
	 *   - GL_DITHER : �����
	 *   - GL_FOG : OpenGL �Ȱ� ���
	 *   - GL_LIGHTING : OpenGL ����
	 *   - GL_LIGHTx : x��° OpenGL ���� (minimum: 8)
	 *   - GL_POINT_SMOOTH : �� ��Ƽ�ٸ����
	 *   - GL_LINE_SMOOTH : �� ��Ƽ�ٸ����
	 *   - GL_LINE_STIPPLE : �� ��Ƽ�ø�
	 *   - GL_POLYGON_SMOOTH : ������ ��Ƽ�ٸ����
	 *   - GL_SCISSOR_TEST : �ü���
	 *   - GL_STENCIL_TEST : ���ٽ� �׽�Ʈ
	 *   - GL_TEXTURE_xD : x������ �ؽ�ó�� (1 ~ 3)
	 *   - GL_TEXTURE_CUBE_MAP : ť��� �ؽ�ó��
	 *   - GL_TEXTURE_GEN_x : x�� ���� texgen (x => { S, T, R, Q })
	 */
	glEnable(GL_BLEND);
	/*
	 * void glBlendFunc(GLenum sfactor, GLenum dfactor);
	 *  - S����(���� ��ϵǴ� ��)�� D����(�̹� ��ϵǾ� �ִ� ��)�� ������ ������� ���� sfactor�� dfactor�� ����.
	 *  - �⺻ ������ S * sfactor + D * dfactor �̴�. (GL_FUNC_ADD)
	 * 
	 * GL_SRC_ALPHA : (S.a, S.a, S.a, S.a)
	 * GL_ONE_MINUS_SRC_ALPHA : (1-S.a, 1-S.a, 1-S.a, 1-S.a)
	 * 
	 * ex. ������ �����ϴ� ��(D)�� (0, 0, 1, 1)�̰� �߰��Ǵ� ��(S)�� (1, 0, 0, 0.4)�� ��
	 * S * sfactor + D * dfactor
	 *   = S * 0.4 + D * (1 - 0.4)
	 *   = (1, 0, 0, 0.4) * 0.4 + (0, 0, 1, 1) * (1 - 0.4)
	 *   = (1 * 0.4 + 0 * 0.6, 0 * 0.4 + 0 * 0.4, 0 * 0.4 + 1 * 0.6, 0.4 * 0.4 + 1 * 0.6)
	 * 
	 * ��������� S�� D�� S�� alpha ���� ���� ���δ�.
	 */
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (!glfwWindowShouldClose(window)) {
		ProcessInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shaderProgram->SetFloat("threshold", sin(glfwGetTime()) * 0.5f + 0.5f);

		box->Draw();

		glfwPollEvents();
		glfwSwapBuffers(window);
	}
#pragma endregion

	// TODO: Ư�� ���̴��� ���� glEnable/glDisable�� ������ �� �ֵ��� �ؾ� �Ѵ�.
	glDisable(GL_BLEND);

	shaderProgram.reset();                                          
	box.reset();
	texture.reset();

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