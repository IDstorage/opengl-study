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

	if (shaders.find(shader.GetType()) == shaders.end()) {
		shaders.emplace(std::make_pair(shader.GetType(), shader.GetObject()));
		return true;
	}
	shaders[shader.GetType()] = shader.GetObject();

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


bool CustomShaderProgram::SetInt(const std::string& name, int value) {
	int location = glGetUniformLocation(programObject, name.c_str());
	if (location == -1) return false;

	// glGetUniformLocation�� glUseProgram�� ȣ���� �ʿ������
	// glUniformXX �迭 �Լ��� glUseProgram�� ȣ���� �ʼ���.
	glUseProgram(programObject);
	glUniform1i(location, value);

	return true;
}

bool CustomShaderProgram::SetUInt(const std::string& name, unsigned int value) {
	int location = glGetUniformLocation(programObject, name.c_str());
	if (location == -1) return false;

	glUseProgram(programObject);
	glUniform1ui(location, value);

	return true;
}

bool CustomShaderProgram::SetFloat(const std::string& name, float value) {
	int location = glGetUniformLocation(programObject, name.c_str());
	if (location == -1) return false;

	glUseProgram(programObject);
	glUniform1f(location, value);

	return true;
}

bool CustomShaderProgram::SetVec3(const std::string& name, float x, float y, float z) {
	int location = glGetUniformLocation(programObject, name.c_str());
	if (location == -1) return false;

	glUseProgram(programObject);
	glUniform3f(location, x, y, z);

	return true;
}

bool CustomShaderProgram::SetVec4(const std::string& name, float x, float y, float z, float w) {
	int location = glGetUniformLocation(programObject, name.c_str());
	if (location == -1) return false;

	glUseProgram(programObject);
	glUniform4f(location, x, y, z, w);

	return true;
}


unsigned int CustomShaderProgram::GetObject() const {
	return programObject;
}