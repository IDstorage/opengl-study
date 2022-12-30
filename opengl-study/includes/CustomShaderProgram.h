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
	bool SetInt(const std::string& name, int value);
	bool SetUInt(const std::string& name, unsigned int value);
	bool SetFloat(const std::string& name, float value);
	bool SetVec3(const std::string& name, float x, float y, float z);
	bool SetVec4(const std::string& name, float x, float y, float z, float w);

public:
	unsigned int GetObject() const;
};