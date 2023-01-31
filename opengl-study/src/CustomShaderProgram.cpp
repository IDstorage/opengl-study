#include "CustomShaderProgram.h"
#include "CustomShader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

// glCreateProgram()으로 shader program 오브젝트를 생성한다.
CustomShaderProgram::CustomShaderProgram() : programObject(glCreateProgram()) {}
CustomShaderProgram::~CustomShaderProgram() {
	// shader program을 다 사용했으면 해제한다.
	glDeleteProgram(programObject);
}


bool CustomShaderProgram::Attach(unsigned int shaderType, const std::string& fileName) {
	auto shader = CustomShader(shaderType);

	shader.LoadShaderFile(fileName);
	if (!shader.CompileShader()) return false;

	return Attach(shader);
}
bool CustomShaderProgram::Attach(const CustomShader& shader) {
	// shader program 오브젝트에 shader를 추가한다.
	// 이때 추가할 shader의 오브젝트 id(uint)를 넘겨준다.
	glAttachShader(programObject, shader.GetObject());

	if (shaders.find(shader.GetType()) == shaders.end()) {
		shaders.emplace(std::make_pair(shader.GetType(), shader.GetObject()));
		return true;
	}
	shaders[shader.GetType()] = shader.GetObject();

	return true;
}

bool CustomShaderProgram::Link() {
	// glAttachShader를 통해 첨부된 shader들을 연결시켜준다.
	glLinkProgram(programObject);

	int success;
	// glGetShaderiv와 동일하다.
	glGetProgramiv(programObject, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		// glGetShaderInfoLog와 동일하다.
		glGetProgramInfoLog(programObject, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER_PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
		return false;
	}

	return true;
}

void CustomShaderProgram::Use() {
	// 현재 shader program state를 넘겨준 shader program으로 변경한다.
	// 이 코드가 실행된 이후 모든 shader와 렌더링 명령은 이 shader program을 사용하게 된다.
	glUseProgram(programObject);
}


bool CustomShaderProgram::SetInt(const std::string& name, int value) {
	int location = glGetUniformLocation(programObject, name.c_str());
	if (location == -1) return false;

	// glGetUniformLocation은 glUseProgram의 호출이 필요없지만
	// glUniformXX 계열 함수는 glUseProgram의 호출이 필수다.
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