#pragma once

#include <string>

class GLSLFile {
private:
	std::string fileName;
	std::string rawData;

public:
	GLSLFile(const std::string& fileName);
	~GLSLFile() = default;

public:
	void Read();
	void Read(const std::string& fileName);

	const char* ToCString() const noexcept;
};