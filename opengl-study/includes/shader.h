#pragma once

#include <memory>
#include <string>

namespace ids {
	class GlslFile;

	class Shader {
	private:
		unsigned int shader_object;
		unsigned int shader_type;

		std::unique_ptr<GlslFile> file;

	public:
		Shader(unsigned int type);
		~Shader();
	public:
		void loadShaderFile(const std::string& path);
		bool compileShader();

	private:
		void sendCompileErrorLog();

	public:
		unsigned int getObject() const;
		unsigned int getType() const;
	};
}