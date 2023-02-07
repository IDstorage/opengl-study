#pragma once

#include <string>
#include <map>

namespace ids {
	class Shader;

	class ShaderProgram {
	private:
		unsigned int program_object;
		std::map<unsigned int, unsigned int> shaders;

	public:
		ShaderProgram();
		~ShaderProgram();

	public:
		bool attach(unsigned int shader_type, const std::string& file_name);
		bool attach(const Shader& shader);

		bool link();

		void use();

	public:
		bool setInt(const std::string& name, int value);
		bool setUInt(const std::string& name, unsigned int value);
		bool setFloat(const std::string& name, float value);
		bool setVec3(const std::string& name, float x, float y, float z);
		bool setVec4(const std::string& name, float x, float y, float z, float w);

	public:
		unsigned int getObject() const;
	};
}