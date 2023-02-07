#include "shader.h"
#include "glsl_file.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

using namespace ids;

Shader::Shader(unsigned int type) : shader_type(type) {
	// Shader 생성
	// GL_VERTEX_SHADER / GL_FRAGEMENT`_SHADER 등을 인자로 줄 수 있다.
	shader_object = glCreateShader(shader_type);
}
Shader::~Shader() {
	// Shader의 사용이 끝나면 glDeleteShader를 통해 지워준다.
	glDeleteShader(shader_object);
}


void Shader::loadShaderFile(const std::string& path) {
	if (file) {
		file->read(path);
		return;
	}
	file = std::make_unique<GlslFile>(path);
}

bool Shader::compileShader() {
	if (file == nullptr) {
		std::cerr << "Target shader file was not detected." << std::endl;
		return false;
	}

	const char* content = file->toCString();
	/*
	 * Shader 소스를 glCreateShader로 만든 오브젝트에 전달
	 *   p1. shader 오브젝트
	 *   p2. 개수
	 *   p3. 소스 (string) 주소
	 */
	glShaderSource(shader_object, 1, &content, NULL);
	// 전달된 shader 소스를 컴파일한다.
	glCompileShader(shader_object);

	int success;
	/*
	 * 컴파일 결과(GL_COMPILE_STATUS)를 받아온다.
	 * iv 접미사는 vector of int이며 옵션에 대한 결과를 GLint* 로 넘긴다.
	 * 옵션은 다음과 같다.
	 *  - GL_SHADER_TYPE
	 *  - GL_DELETE_STATUS
	 *  - GL_COMPILE_STATUS
	 *  - GL_INFO_LOG_LENGTH
	 *  - GL_SHADER_SOURCE_LENGTH
	 * 각 옵션마다 GLint*로 반환되는 값이 다르다. GL_COMPILE_STATUS는 GL_TRUE | GL_FALSE를 반환한다.
	 */
	glGetShaderiv(shader_object, GL_COMPILE_STATUS, &success);

	if (success) return true;

	sendCompileErrorLog();
	return false;
}

void Shader::sendCompileErrorLog() {
	char info_log[512];
	// Shader를 컴파일한 후 로그를 수집한다.
	glGetShaderInfoLog(shader_object, 512, NULL, info_log);
	std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
}


unsigned int Shader::getObject() const { 
	return shader_object;
}

unsigned int Shader::getType() const {
	return shader_type;
}