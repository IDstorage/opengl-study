#include "CustomShaderProgram.h"
#include "CustomShader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

CustomShaderProgram::CustomShaderProgram() : programObject(glCreateProgram()) {}
CustomShaderProgram::~CustomShaderProgram() {
	glDeleteProgram(programObject);
}


bool CustomShaderProgram::Attach(unsigned int shaderType, const std::string& fileName) {
	auto shader = CustomShader(shaderType);
	shader.LoadShaderFile(fileName);
	if (!shader.CompileShader()) return false;
	return Attach(shader);
}
bool CustomShaderProgram::Attach(const CustomShader& shader) {
	glAttachShader(programObject, shader.GetObject());
	return true;
}

bool CustomShaderProgram::Link() {
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


unsigned int CustomShaderProgram::GetObject() const {
	return programObject;
}