#pragma once

#include "Texture.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "stb_image.h"


Texture::Texture(const std::string& fileName, bool mipmap) : texName(fileName), hasMipmap(mipmap) {
	glGenTextures(1, &textureObject);

	Load();
}


void Texture::Load(const std::string& file, bool mipmap) {
	if (!file.empty()) {
		texName = file;
	}

	if (texName.empty()) {
		return;
	}

	hasMipmap = mipmap;

	glBindTexture(GL_TEXTURE_2D, textureObject);

	// OpenGL�� �̹����� (0, 0)�� �Ʒ��� �ν��ؼ� ������ ����.
	// ���� �̹����� (0, 0)�� �»���̱� ������ �ҷ��� �� ������ �� �ְ� ��.
	stbi_set_flip_vertically_on_load(true);

	unsigned char* data = stbi_load(texName.c_str(), &width, &height, &channels, 0);
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
	if (hasMipmap) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	stbi_image_free(data);
}


void Texture::SetIndex(int index) {
	targetIndex = index;
}

void Texture::SetWrapOption(int sOption, int tOption) {
	glBindTexture(GL_TEXTURE_2D, textureObject);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sOption);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tOption);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetMinifyFilter(int option) {
	glBindTexture(GL_TEXTURE_2D, textureObject);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, option);

	glBindTexture(GL_TEXTURE_2D, 0);
}
void Texture::SetMagnifyFilter(int option) {
	glBindTexture(GL_TEXTURE_2D, textureObject);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, option);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Use() {
	if (targetIndex.has_value()) {
		glActiveTexture(targetIndex.value());
	}
	glBindTexture(GL_TEXTURE_2D, textureObject);
}