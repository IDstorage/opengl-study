#include "shaderprogram.h"
#include "shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

using namespace ids;

// glCreateProgram()으로 shader program 오브젝트를 생성한다.
ShaderProgram::ShaderProgram() : program_object(glCreateProgram()) {}
ShaderProgram::~ShaderProgram() {
	// shader program을 다 사용했으면 해제한다.
	glDeleteProgram(program_object);
}


bool ShaderProgram::attach(unsigned int shaderType, const std::string& fileName) {
	auto shader = Shader(shaderType);

	shader.loadShaderFile(fileName);
	if (!shader.compileShader()) return false;

	return attach(shader);
}
bool ShaderProgram::attach(const Shader& shader) {
	// shader program 오브젝트에 shader를 추가한다.
	// 이때 추가할 shader의 오브젝트 id(uint)를 넘겨준다.
	glAttachShader(program_object, shader.getObject());

	if (shaders.find(shader.getType()) == shaders.end()) {
		shaders.emplace(std::make_pair(shader.getType(), shader.getObject()));
		return true;
	}
	shaders[shader.getType()] = shader.getObject();

	return true;
}

bool ShaderProgram::link() {
	// glAttachShader를 통해 첨부된 shader들을 연결시켜준다.
	glLinkProgram(program_object);

	int success;
	// glGetShaderiv와 동일하다.
	glGetProgramiv(program_object, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		// glGetShaderInfoLog와 동일하다.
		glGetProgramInfoLog(program_object, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER_PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
		return false;
	}

	return true;
}

void ShaderProgram::use() {
	// 현재 shader program state를 넘겨준 shader program으로 변경한다.
	// 이 코드가 실행된 이후 모든 shader와 렌더링 명령은 이 shader program을 사용하게 된다.
	glUseProgram(program_object);
}


bool ShaderProgram::setInt(const std::string& name, int value) {
	int location = glGetUniformLocation(program_object, name.c_str());
	if (location == -1) return false;

	// glGetUniformLocation은 glUseProgram의 호출이 필요없지만
	// glUniformXX 계열 함수는 glUseProgram의 호출이 필수다.
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