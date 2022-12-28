#include "CustomShader.h"
#include "GLSLFile.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

CustomShader::CustomShader(unsigned int type) : shaderType(type) {
	// Shader 생성
	// GL_VERTEX_SHADER / GL_FRAGEMENT_SHADER 등을 인자로 줄 수 있다.
	shaderObject = glCreateShader(shaderType);
}
CustomShader::~CustomShader() {
	// Shader의 사용이 끝나면 glDeleteShader를 통해 지워준다.
	glDeleteShader(shaderObject);
}


void CustomShader::LoadShaderFile(const std::string& path) {
	if (file) {
		file->Read(path);
		return;
	}
	file = std::make_unique<GLSLFile>(path);
}

bool CustomShader::CompileShader() {
	if (file == nullptr) {
		std::cerr << "Target shader file was not detected." << std::endl;
		return false;
	}

	const char* content = file->ToCString();
	/*
	 * Shader 소스를 glCreateShader로 만든 오브젝트에 전달
	 *   p1. shader 오브젝트
	 *   p2. 개수
	 *   p3. 소스 (string) 주소
	 */
	glShaderSource(shaderObject, 1, &content, NULL);
	// 전달된 shader 소스를 컴파일한다.
	glCompileShader(shaderObject);

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
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &success);

	if (success) return true;

	SendCompileErrorLog();
	return false;
}

void CustomShader::SendCompileErrorLog() {
	char infoLog[512];
	// Shader를 컴파일한 후 로그를 수집한다.
	glGetShaderInfoLog(shaderObject, 512, NULL, infoLog);
	std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
}


unsigned int CustomShader::GetObject() const { 
	return shaderObject;
}

unsigned int CustomShader::GetType() const {
	return shaderType;
}