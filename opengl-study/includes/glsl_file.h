#pragma once

#include <string>

namespace ids {
	class GlslFile {
	private:
		std::string file_name;
		std::string raw_data;

	public:
		GlslFile(const std::string& file_name);
		~GlslFile() = default;

	public:
		void read();
		void read(const std::string& file_name);

		const char* toCString() const noexcept;
	};
}