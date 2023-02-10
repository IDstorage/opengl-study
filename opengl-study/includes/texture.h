#pragma once

#include <string>
#include <optional>

namespace ids {
	class Texture {
	private:
		std::string tex_name;

		int width;
		int height;
		int channels;

		bool has_mipmap;

		unsigned int texture_object;

		std::optional<int> target_index;

	public:
		Texture() = default;
		Texture(const std::string& file_name, bool mipmap = true);

	public:
		void load(const std::string& file = "", bool mipmap = true);

	public:
		void setIndex(int index);

		void setWrapOption(int s_option, int t_0ption);

		void setMinifyFilter(int option);
		void setMagnifyFilter(int option);

		void use();
	};
}