#include "Common/GLSLFile.h"
#include <fstream>
#include <string>

GLSLFile::GLSLFile(const std::string& fileName) {
	Read(fileName);
}


void GLSLFile::Read() {
	Read(fileName);
}
void GLSLFile::Read(const std::string & fileName) {
	this->fileName = fileName;
	std::ifstream file(this->fileName, std::ios::in);
	if (file.is_open()) {
		std::string line;
		while (getline(file, line)) {
			rawData = rawData.append(line + '\n');
		}
		file.close();
	}
}


const char* GLSLFile::ToCString() const noexcept {
	return rawData.c_str();
}