#include "CustomShaderProgram.h"
#include "CustomShader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

// glCreateProgram()���� shader program ������Ʈ�� �����Ѵ�.
CustomShaderProgram::CustomShaderProgram() : programObject(glCreateProgram()) {}
CustomShaderProgram::~CustomShaderProgram() {
	// shader program�� �� ��������� �����Ѵ�.
	glDeleteProgram(programObject);
}


bool CustomShaderProgram::Attach(unsigned int shaderType, const std::string& fileName) {
	auto shader = CustomShader(shaderType);
	shader.LoadShaderFile(fileName);
	if (!shader.CompileShader()) return false;
	return Attach(shader);
}
bool CustomShaderProgram::Attach(const CustomShader& shader) {
	// shader program ������Ʈ�� shader�� �߰��Ѵ�.
	// �̶� �߰��� shader�� ������Ʈ id(uint)�� �Ѱ��ش�.
	glAttachShader(programObject, shader.GetObject());
	return true;
}

bool CustomShaderProgram::Link() {
	// glAttachShader�� ���� ÷�ε� shader���� ��������ش�.
	glLinkProgram(programObject);

	int success;
	// glGetShaderiv�� �����ϴ�.
	glGetProgramiv(programObject, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		// glGetShaderInfoLog�� �����ϴ�.
		glGetProgramInfoLog(programObject, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER_PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
		return false;
	}

	return true;
}

void CustomShaderProgram::Use() {
	// ���� shader program state�� �Ѱ��� shader program���� �����Ѵ�.
	// �� �ڵ尡 ����� ���� ��� shader�� ������ ����� �� shader program�� ����ϰ� �ȴ�.
	glUseProgram(programObject);
}


unsigned int CustomShaderProgram::GetObject() const {
	return programObject;
}