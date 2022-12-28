#pragma once

#include <string>

class CustomShader;

class CustomShaderProgram {
private:
	unsigned int programObject;

public:
	CustomShaderProgram();
	~CustomShaderProgram();

public:
	bool Attach(unsigned int shaderType, const std::string& fileName);
	bool Attach(const CustomShader& shader);

	bool Link();

	void Use();

public:
	unsigned int GetObject() const;
};