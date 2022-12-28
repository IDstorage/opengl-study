#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

void OnResizeCallback(GLFWwindow*, int, int);
void ProcessInput(GLFWwindow*);

class GLSLFile {
private:
	std::string fileName;
	std::string rawData;

public:
	GLSLFile(const std::string& fileName) {
		Read(fileName);
	}
	~GLSLFile() = default;
	
public:
	void Read() {
		Read(fileName);
	}
	void Read(const std::string& fileName) {
		this->fileName = fileName;
		std::ifstream file(this->fileName, std::ios::in);
		if (file.is_open()) {
			std::string line;
			while (getline(file, line)) {
				rawData = rawData.append(line + '\n');
			}
			file.close();
		}
	}

	const char* ToCString() const noexcept {
		return rawData.c_str();
	}
};

class CustomShader {
private:
	unsigned int shaderObject;
	GLenum shaderType;

	std::unique_ptr<GLSLFile> file;
	
public:
	CustomShader(GLenum type) : shaderType(type) {
		shaderObject = glCreateShader(shaderType);
	}
	~CustomShader() {
		glDeleteShader(shaderObject);
	}
public:
	void LoadShaderFile(const std::string& path) {
		if (file) {
			file->Read(path);
			return;
		}
		file = std::make_unique<GLSLFile>(path);
	}

	bool CompileShader() {
		if (file == nullptr) {
			std::cerr << "Target shader file was not detected." << std::endl;
			return false;
		}
		
		const char* content = file->ToCString();
		glShaderSource(shaderObject, 1, &content, NULL);
		glCompileShader(shaderObject);

		int success;
		glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &success);

		if (success) return true;

		SendCompileErrorLog();
		return false;
	}
private:
	void SendCompileErrorLog() {
		char infoLog[512];
		glGetShaderInfoLog(shaderObject, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

public:
	unsigned int GetObject() const { return shaderObject; }
	GLenum GetType() const { return shaderType; }
};


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
	auto vertexShader = std::make_unique<CustomShader>(GL_VERTEX_SHADER);
	vertexShader->LoadShaderFile("./01_Triangle.vert");
	bool ret = vertexShader->CompileShader();
	if (!ret) {
		glfwTerminate();
		return -1;
	}

	auto fragmentShader = std::make_unique<CustomShader>(GL_FRAGMENT_SHADER);
	fragmentShader->LoadShaderFile("./01_Triangle.frag");
	ret = fragmentShader->CompileShader();
	if (!ret) {
		glfwTerminate();
		return -1;
	}

	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader->GetObject());
	glAttachShader(shaderProgram, fragmentShader->GetObject());
	glLinkProgram(shaderProgram);

	int success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER_PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
		glfwTerminate();
		return -1;
	}

	vertexShader.reset();
	fragmentShader.reset();
#pragma endregion

	/* 
	 * NDC(Normalized Device Coordinates)
	 *  - 모든 축에 대해 값의 범위가 [-1, 1]로 국한되며 벗어난 값은 렌더링 되지 않는다.
	 *  - 좌하단 (-1, -1) / 우상단 (1, 1)
	 * 
	 * 이 NDC는 glViewport 함수에 전달한 데이터(픽셀 수)를 바탕으로 Screen-space coordinates(화면 좌표)로 변환된다. (Viewport transform, 뷰포트 변환)
	 * 그렇게 변환된 screen-space coordinates는 fragment로 변환되어 fragment shader의 입력으로 전달된다.
	 */
	float vertices[] = { -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f };

	/*
	 * 정점 데이터를 vertex shader에 전달해야 한다.
	 *  - GPU에 정점 데이터를 저장할 공간 할당
	 *  - OpenGL이 어떻게 메모리를 해석할 것인지 구성
	 *  - 데이터를 어떻게 그래픽 카드에 전달할 것인지 명시
	 */
	unsigned int vbo, vao; // Vertex Buffer Object / Vertex Array Object
	glGenBuffers(1, &vbo); // 고유한 버퍼 ID를 생성
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);

	// GL_ARRAY_BUFFER에 vbo 바인드
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	/* 데이터를 현재 바인딩된 버퍼에 복사하는 기능(아까 glBindBuffer때문에 GL_ARRAY_BUFFER는 vbo를 가리킴)
	 * 마지막 인자는 그래픽 카드가 데이터를 관리하는 방법
	 *  - GL_STATIC_DRAW : 데이터가 거의 변하지 않음
	 *  - GL_DYNAMIC_DRAW : 데이터가 자주 변경됨
	 *  - GL_STREAM_DRAW : 데이터가 그려질 때마다 변경됨
	 */
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);


	while (!glfwWindowShouldClose(window)) {
		ProcessInput(window);

		glUseProgram(shaderProgram);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteProgram(shaderProgram);

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