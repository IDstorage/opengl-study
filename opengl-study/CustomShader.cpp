#include "CustomShader.h"
#include "GLSLFile.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

CustomShader::CustomShader(unsigned int type) : shaderType(type) {
	// Shader ����
	// GL_VERTEX_SHADER / GL_FRAGEMENT_SHADER ���� ���ڷ� �� �� �ִ�.
	shaderObject = glCreateShader(shaderType);
}
CustomShader::~CustomShader() {
	// Shader�� ����� ������ glDeleteShader�� ���� �����ش�.
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
	 * Shader �ҽ��� glCreateShader�� ���� ������Ʈ�� ����
	 *   p1. shader ������Ʈ
	 *   p2. ����
	 *   p3. �ҽ� (string) �ּ�
	 */
	glShaderSource(shaderObject, 1, &content, NULL);
	// ���޵� shader �ҽ��� �������Ѵ�.
	glCompileShader(shaderObject);

	int success;
	/*
	 * ������ ���(GL_COMPILE_STATUS)�� �޾ƿ´�.
	 * iv ���̻�� vector of int�̸� �ɼǿ� ���� ����� GLint* �� �ѱ��.
	 * �ɼ��� ������ ����.
	 *  - GL_SHADER_TYPE
	 *  - GL_DELETE_STATUS
	 *  - GL_COMPILE_STATUS
	 *  - GL_INFO_LOG_LENGTH
	 *  - GL_SHADER_SOURCE_LENGTH
	 * �� �ɼǸ��� GLint*�� ��ȯ�Ǵ� ���� �ٸ���. GL_COMPILE_STATUS�� GL_TRUE | GL_FALSE�� ��ȯ�Ѵ�.
	 */
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &success);

	if (success) return true;

	SendCompileErrorLog();
	return false;
}

void CustomShader::SendCompileErrorLog() {
	char infoLog[512];
	// Shader�� �������� �� �α׸� �����Ѵ�.
	glGetShaderInfoLog(shaderObject, 512, NULL, infoLog);
	std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
}


unsigned int CustomShader::GetObject() const { 
	return shaderObject;
}

unsigned int CustomShader::GetType() const {
	return shaderType;
}