#pragma once

#include "texture.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "stb_image.h"

using namespace ids;

Texture::Texture(const std::string& file_name, bool mipmap) : tex_name(file_name), has_mipmap(mipmap) {
	glGenTextures(1, &texture_object);

	load();
}


void Texture::load(const std::string& file, bool mipmap) {
	if (!file.empty()) {
		tex_name = file;
	}

	if (tex_name.empty()) {
		return;
	}

	has_mipmap = mipmap;

	glBindTexture(GL_TEXTURE_2D, texture_object);

	// OpenGL이 이미지의 (0, 0)을 아래로 인식해서 뒤집혀 나옴.
	// 실제 이미지의 (0, 0)은 좌상단이기 때문에 불러올 때 뒤집을 수 있게 함.
	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load(tex_name.c_str(), &width, &height, &channels, 0);
	// channels
	// 1 : grey
	// 2 : grey, alpha
	// 3 : red, green, blue
	// 4 : red, green, blue, alpha
	bool hasAlpha = channels == 2 || channels == 4;

	if (!data) {
		std::cout << "Failed to load texture" << std::endl << stbi_failure_reason() << std::endl;
		return;
	}

	auto type = hasAlpha ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, data);
	if (has_mipmap) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	stbi_image_free(data);
}


void Texture::setIndex(int index) {
	target_index = index;
}

void Texture::setWrapOption(int s_option, int t_option) {
	glBindTexture(GL_TEXTURE_2D, texture_object);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s_option);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t_option);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setMinifyFilter(int option) {
	glBindTexture(GL_TEXTURE_2D, texture_object);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, option);

	glBindTexture(GL_TEXTURE_2D, 0);
}
void Texture::setMagnifyFilter(int option) {
	glBindTexture(GL_TEXTURE_2D, texture_object);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, option);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::use() {
	if (target_index.has_value()) {
		glActiveTexture(target_index.value());
	}
	glBindTexture(GL_TEXTURE_2D, texture_object);
}