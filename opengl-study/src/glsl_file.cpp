#include "glsl_file.h"
#include <fstream>
#include <string>

using namespace ids;

GlslFile::GlslFile(const std::string& file_name) {
	read(file_name);
}


void GlslFile::read() {
	read(file_name);
}
void GlslFile::read(const std::string & file_name) {
	this->file_name = file_name;
	std::ifstream file(this->file_name, std::ios::in);
	if (file.is_open()) {
		std::string line;
		while (getline(file, line)) {
			raw_data = raw_data.append(line + '\n');
		}
		file.close();
	}
}


const char* GlslFile::toCString() const noexcept {
	return raw_data.c_str();
}