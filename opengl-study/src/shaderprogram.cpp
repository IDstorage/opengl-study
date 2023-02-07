#include "shaderprogram.h"
#include "shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

using namespace ids;

// glCreateProgram()���� shader program ������Ʈ�� �����Ѵ�.
ShaderProgram::ShaderProgram() : program_object(glCreateProgram()) {}
ShaderProgram::~ShaderProgram() {
	// shader program�� �� ��������� �����Ѵ�.
	glDeleteProgram(program_object);
}


bool ShaderProgram::attach(unsigned int shaderType, const std::string& fileName) {
	auto shader = Shader(shaderType);

	shader.loadShaderFile(fileName);
	if (!shader.compileShader()) return false;

	return attach(shader);
}
bool ShaderProgram::attach(const Shader& shader) {
	// shader program ������Ʈ�� shader�� �߰��Ѵ�.
	// �̶� �߰��� shader�� ������Ʈ id(uint)�� �Ѱ��ش�.
	glAttachShader(program_object, shader.getObject());

	if (shaders.find(shader.getType()) == shaders.end()) {
		shaders.emplace(std::make_pair(shader.getType(), shader.getObject()));
		return true;
	}
	shaders[shader.getType()] = shader.getObject();

	return true;
}

bool ShaderProgram::link() {
	// glAttachShader�� ���� ÷�ε� shader���� ��������ش�.
	glLinkProgram(program_object);

	int success;
	// glGetShaderiv�� �����ϴ�.
	glGetProgramiv(program_object, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		// glGetShaderInfoLog�� �����ϴ�.
		glGetProgramInfoLog(program_object, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER_PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
		return false;
	}

	return true;
}

void ShaderProgram::use() {
	// ���� shader program state�� �Ѱ��� shader program���� �����Ѵ�.
	// �� �ڵ尡 ����� ���� ��� shader�� ������ ����� �� shader program�� ����ϰ� �ȴ�.
	glUseProgram(program_object);
}


bool ShaderProgram::setInt(const std::string& name, int value) {
	int location = glGetUniformLocation(program_object, name.c_str());
	if (location == -1) return false;

	// glGetUniformLocation�� glUseProgram�� ȣ���� �ʿ������
	// glUniformXX �迭 �Լ��� glUseProgram�� ȣ���� �ʼ���.
	glUseProgram(program_object);
	glUniform1i(location, value);

	return true;
}

bool ShaderProgram::setUInt(const std::string& name, unsigned int value) {
	int location = glGetUniformLocation(program_object, name.c_str());
	if (location == -1) return false;

	glUseProgram(program_object);
	glUniform1ui(location, value);

	return true;
}

bool ShaderProgram::setFloat(const std::string& name, float value) {
	int location = glGetUniformLocation(program_object, name.c_str());
	if (location == -1) return false;

	glUseProgram(program_object);
	glUniform1f(location, value);

	return true;
}

bool ShaderProgram::setVec3(const std::string& name, float x, float y, float z) {
	int location = glGetUniformLocation(program_object, name.c_str());
	if (location == -1) return false;

	glUseProgram(program_object);
	glUniform3f(location, x, y, z);

	return true;
}

bool ShaderProgram::setVec4(const std::string& name, float x, float y, float z, float w) {
	int location = glGetUniformLocation(program_object, name.c_str());
	if (location == -1) return false;

	glUseProgram(program_object);
	glUniform4f(location, x, y, z, w);

	return true;
}


unsigned int ShaderProgram::getObject() const {
	return program_object;
}