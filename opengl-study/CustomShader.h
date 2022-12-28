#pragma once

#include <memory>
#include <string>

class GLSLFile;

class CustomShader {
private:
	unsigned int shaderObject;
	unsigned int shaderType;

	std::unique_ptr<GLSLFile> file;

public:
	CustomShader(unsigned int type);
	~CustomShader();
public:
	void LoadShaderFile(const std::string& path);
	bool CompileShader();

private:
	void SendCompileErrorLog();

public:
	unsigned int GetObject() const;
	unsigned int GetType() const;
};