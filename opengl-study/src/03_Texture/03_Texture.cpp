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
	 * GL_BLEND(블렌딩) 기능을 사용한다. glDisable(...)과 함께 사용한다.
	 * 
	 * 옵션으로는
	 *   - GL_BLEND : 색상 블렌딩
	 *   - GL_CULL_FACE : 폴리곤 추려내기
	 *   - GL_DEPTH_TEST : 깊이 테스트
	 *   - GL_DITHER : 디더링
	 *   - GL_FOG : OpenGL 안개 모드
	 *   - GL_LIGHTING : OpenGL 조명
	 *   - GL_LIGHTx : x번째 OpenGL 조명 (minimum: 8)
	 *   - GL_POINT_SMOOTH : 점 안티앨리어싱
	 *   - GL_LINE_SMOOTH : 선 안티앨리어싱
	 *   - GL_LINE_STIPPLE : 선 스티플링
	 *   - GL_POLYGON_SMOOTH : 폴리곤 안티앨리어싱
	 *   - GL_SCISSOR_TEST : 시서링
	 *   - GL_STENCIL_TEST : 스텐실 테스트
	 *   - GL_TEXTURE_xD : x차원의 텍스처링 (1 ~ 3)
	 *   - GL_TEXTURE_CUBE_MAP : 큐브맵 텍스처링
	 *   - GL_TEXTURE_GEN_x : x에 대한 texgen (x => { S, T, R, Q })
	 */
	glEnable(GL_BLEND);
	/*
	 * void glBlendFunc(GLenum sfactor, GLenum dfactor);
	 *  - S색상(새로 기록되는 값)과 D색상(이미 기록되어 있는 값)에 적용할 연산식을 각각 sfactor와 dfactor로 정의.
	 *  - 기본 연산은 S * sfactor + D * dfactor 이다. (GL_FUNC_ADD)
	 * 
	 * GL_SRC_ALPHA : (S.a, S.a, S.a, S.a)
	 * GL_ONE_MINUS_SRC_ALPHA : (1-S.a, 1-S.a, 1-S.a, 1-S.a)
	 * 
	 * ex. 기존에 존재하던 색(D)이 (0, 0, 1, 1)이고 추가되는 색(S)이 (1, 0, 0, 0.4)일 때
	 * S * sfactor + D * dfactor
	 *   = S * 0.4 + D * (1 - 0.4)
	 *   = (1, 0, 0, 0.4) * 0.4 + (0, 0, 1, 1) * (1 - 0.4)
	 *   = (1 * 0.4 + 0 * 0.6, 0 * 0.4 + 0 * 0.4, 0 * 0.4 + 1 * 0.6, 0.4 * 0.4 + 1 * 0.6)
	 * 
	 * 결과적으로 S와 D가 S의 alpha 값에 의해 섞인다.
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

	// TODO: 특정 셰이더에 대해 glEnable/glDisable을 관리할 수 있도록 해야 한다.
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