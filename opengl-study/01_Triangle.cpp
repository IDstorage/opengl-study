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
		glGenBuffers(1, &vbo); // 고유한 버퍼 ID를 생성
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
	 * 정점 데이터를 vertex shader에 전달해야 한다.
	 *  - GPU에 정점 데이터를 저장할 공간 할당
	 *  - OpenGL이 어떻게 메모리를 해석할 것인지 구성
	 *  - 데이터를 어떻게 그래픽 카드에 전달할 것인지 명시
	 */
	void SetVertices(const std::initializer_list<float>& verts) {
		vertices = verts;

		auto vertsArray = vertices.data();

		// vao를 VertexArray에 바인딩하고 시작
		glBindVertexArray(vao);

		// GL_ARRAY_BUFFER에 vbo 바인드
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		/* 데이터를 현재 바인딩된 버퍼에 복사하는 기능(아까 glBindBuffer때문에 GL_ARRAY_BUFFER는 vbo를 가리킴)
		 * 마지막 인자는 그래픽 카드가 데이터를 관리하는 방법
		 *  - GL_STATIC_DRAW : 데이터가 거의 변하지 않음
		 *  - GL_DYNAMIC_DRAW : 데이터가 자주 변경됨
		 *  - GL_STREAM_DRAW : 데이터가 그려질 때마다 변경됨
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
	 *  - 모든 축에 대해 값의 범위가 [-1, 1]로 국한되며 벗어난 값은 렌더링 되지 않는다.
	 *  - 좌하단 (-1, -1) / 우상단 (1, 1)
	 * 
	 * 이 NDC는 glViewport 함수에 전달한 데이터(픽셀 수)를 바탕으로 Screen-space coordinates(화면 좌표)로 변환된다. (Viewport transform, 뷰포트 변환)
	 * 그렇게 변환된 screen-space coordinates는 fragment로 변환되어 fragment shader의 입력으로 전달된다.
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