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

class CustomShaderProgram {
private:
	unsigned int programObject;
	
public:
	CustomShaderProgram() : programObject(glCreateProgram()) {}
	~CustomShaderProgram() {
		glDeleteProgram(programObject);
	}

public:
	bool Attach(GLenum shaderType, const std::string& fileName) {
		auto shader = CustomShader(shaderType);
		shader.LoadShaderFile(fileName);
		if (!shader.CompileShader()) return false;
		return Attach(shader);
	}
	bool Attach(const CustomShader& shader) {
		glAttachShader(programObject, shader.GetObject());
		return true;
	}

	bool Link() {
		glLinkProgram(programObject);

		int success;
		glGetProgramiv(programObject, GL_LINK_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetProgramInfoLog(programObject, 512, NULL, infoLog);
			std::cerr << "ERROR::SHADER_PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
			return false;
		}

		return true;
	}

public:
	unsigned int GetObject() const {
		return programObject;
	}
};

class StaticPolygon {
private:
	std::vector<float> vertices;
	std::vector<int> indices;

	// Vertex Buffer Object / Vertex Array Object / Element array Buffer Object
	unsigned int vao, vbo, ebo;

	bool shouldWireframe;

public:
	StaticPolygon() : shouldWireframe(false) {
		glGenBuffers(1, &vbo); // ������ ���� ID�� ����
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &ebo);
	}
	~StaticPolygon() {
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
	}

public:
	/*
	 * ���� �����͸� vertex shader�� �����ؾ� �Ѵ�.
	 *  - GPU�� ���� �����͸� ������ ���� �Ҵ�
	 *  - OpenGL�� ��� �޸𸮸� �ؼ��� ������ ����
	 *  - �����͸� ��� �׷��� ī�忡 ������ ������ ���
	 */
	void SetVertices(const std::initializer_list<float>& verts) {
		vertices = verts;

		auto vertsArray = vertices.data();

		// vao�� VertexArray�� ���ε��ϰ� ����
		glBindVertexArray(vao);

		// GL_ARRAY_BUFFER�� vbo ���ε�
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		/* �����͸� ���� ���ε��� ���ۿ� �����ϴ� ���(�Ʊ� glBindBuffer������ GL_ARRAY_BUFFER�� vbo�� ����Ŵ)
		 * ������ ���ڴ� �׷��� ī�尡 �����͸� �����ϴ� ���
		 *  - GL_STATIC_DRAW : �����Ͱ� ���� ������ ����
		 *  - GL_DYNAMIC_DRAW : �����Ͱ� ���� �����
		 *  - GL_STREAM_DRAW : �����Ͱ� �׷��� ������ �����
		 */
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertsArray, GL_STATIC_DRAW);
	}

	void SetIndices(const std::initializer_list<int>& ind) {
		indices = ind;

		auto indArray = indices.data();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indArray, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
		glEnableVertexAttribArray(0);
	}

	void SetWireframeMode(bool active) {
		shouldWireframe = active;
	}

public:
	void Draw() {
		glPolygonMode(GL_FRONT_AND_BACK, shouldWireframe ? GL_LINE : GL_FILL);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, indices.size() * sizeof(int), GL_UNSIGNED_INT, 0);
	}
};


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
	bool ret = shaderProgram->Attach(GL_VERTEX_SHADER, "./01_Triangle.vert");
	ret &= shaderProgram->Attach(GL_FRAGMENT_SHADER, "./01_Triangle.frag");
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

	/* 
	 * NDC(Normalized Device Coordinates)
	 *  - ��� �࿡ ���� ���� ������ [-1, 1]�� ���ѵǸ� ��� ���� ������ ���� �ʴ´�.
	 *  - ���ϴ� (-1, -1) / ���� (1, 1)
	 * 
	 * �� NDC�� glViewport �Լ��� ������ ������(�ȼ� ��)�� �������� Screen-space coordinates(ȭ�� ��ǥ)�� ��ȯ�ȴ�. (Viewport transform, ����Ʈ ��ȯ)
	 * �׷��� ��ȯ�� screen-space coordinates�� fragment�� ��ȯ�Ǿ� fragment shader�� �Է����� ���޵ȴ�.
	 */
	auto leftTriangle = std::make_unique<StaticPolygon>();
	leftTriangle->SetVertices({
		-0.525f, 0.525f, 0.0f,	// left-top
		-0.525f, -0.475f, 0.0f,	// left-bottom
		0.475f, 0.525f, 0.0f		// right-top
	});
	leftTriangle->SetIndices({
		//0, 1, 2		// Counter-Clockwise
		0, 2, 1	// Clockwise
	});

	auto rightTriangle = std::make_unique<StaticPolygon>();
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

		glUseProgram(shaderProgram->GetObject());

		leftTriangle->SetWireframeMode(isWFKeyPressed);
		rightTriangle->SetWireframeMode(!isWFKeyPressed);

		leftTriangle->Draw();
		rightTriangle->Draw();

		glfwPollEvents();
		glfwSwapBuffers(window);
	}
#pragma endregion

	shaderProgram.reset();

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