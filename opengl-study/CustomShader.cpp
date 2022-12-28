#include "CustomShader.h"
#include "GLSLFile.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

CustomShader::CustomShader(unsigned int type) : shaderType(type) {
	shaderObject = glCreateShader(shaderType);
}
CustomShader::~CustomShader() {
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
	glShaderSource(shaderObject, 1, &content, NULL);
	glCompileShader(shaderObject);

	int success;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &success);

	if (success) return true;

	SendCompileErrorLog();
	return false;
}

void CustomShader::SendCompileErrorLog() {
	char infoLog[512];
	glGetShaderInfoLog(shaderObject, 512, NULL, infoLog);
	std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
}


unsigned int CustomShader::GetObject() const { 
	return shaderObject;
}

unsigned int CustomShader::GetType() const {
	return shaderType;
}