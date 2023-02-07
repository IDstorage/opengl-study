#include "shader.h"
#include "glsl_file.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

using namespace ids;

Shader::Shader(unsigned int type) : shader_type(type) {
	// Shader ����
	// GL_VERTEX_SHADER / GL_FRAGEMENT`_SHADER ���� ���ڷ� �� �� �ִ�.
	shader_object = glCreateShader(shader_type);
}
Shader::~Shader() {
	// Shader�� ����� ������ glDeleteShader�� ���� �����ش�.
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
	 * Shader �ҽ��� glCreateShader�� ���� ������Ʈ�� ����
	 *   p1. shader ������Ʈ
	 *   p2. ����
	 *   p3. �ҽ� (string) �ּ�
	 */
	glShaderSource(shader_object, 1, &content, NULL);
	// ���޵� shader �ҽ��� �������Ѵ�.
	glCompileShader(shader_object);

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
	glGetShaderiv(shader_object, GL_COMPILE_STATUS, &success);

	if (success) return true;

	sendCompileErrorLog();
	return false;
}

void Shader::sendCompileErrorLog() {
	char info_log[512];
	// Shader�� �������� �� �α׸� �����Ѵ�.
	glGetShaderInfoLog(shader_object, 512, NULL, info_log);
	std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
}


unsigned int Shader::getObject() const { 
	return shader_object;
}

unsigned int Shader::getType() const {
	return shader_type;
}