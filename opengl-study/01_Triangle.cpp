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
	 *  - ��� �࿡ ���� ���� ������ [-1, 1]�� ���ѵǸ� ��� ���� ������ ���� �ʴ´�.
	 *  - ���ϴ� (-1, -1) / ���� (1, 1)
	 * 
	 * �� NDC�� glViewport �Լ��� ������ ������(�ȼ� ��)�� �������� Screen-space coordinates(ȭ�� ��ǥ)�� ��ȯ�ȴ�. (Viewport transform, ����Ʈ ��ȯ)
	 * �׷��� ��ȯ�� screen-space coordinates�� fragment�� ��ȯ�Ǿ� fragment shader�� �Է����� ���޵ȴ�.
	 */
	float vertices[] = { -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f };

	/*
	 * ���� �����͸� vertex shader�� �����ؾ� �Ѵ�.
	 *  - GPU�� ���� �����͸� ������ ���� �Ҵ�
	 *  - OpenGL�� ��� �޸𸮸� �ؼ��� ������ ����
	 *  - �����͸� ��� �׷��� ī�忡 ������ ������ ���
	 */
	unsigned int vbo, vao; // Vertex Buffer Object / Vertex Array Object
	glGenBuffers(1, &vbo); // ������ ���� ID�� ����
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);

	// GL_ARRAY_BUFFER�� vbo ���ε�
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	/* �����͸� ���� ���ε��� ���ۿ� �����ϴ� ���(�Ʊ� glBindBuffer������ GL_ARRAY_BUFFER�� vbo�� ����Ŵ)
	 * ������ ���ڴ� �׷��� ī�尡 �����͸� �����ϴ� ���
	 *  - GL_STATIC_DRAW : �����Ͱ� ���� ������ ����
	 *  - GL_DYNAMIC_DRAW : �����Ͱ� ���� �����
	 *  - GL_STREAM_DRAW : �����Ͱ� �׷��� ������ �����
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