#pragma once

#include <string>
#include <map>
#include "glm_required.h"

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
		bool setVec3(const std::string& name, const glm::vec3& v3);
		bool setVec3(const std::string& name, float x, float y, float z);
		bool setVec4(const std::string& name, const glm::vec4& v4);
		bool setVec4(const std::string& name, float x, float y, float z, float w);
		bool setMat4(const std::string& name, const glm::mat4& m);

	public:
		unsigned int getObject() const;
	};
}