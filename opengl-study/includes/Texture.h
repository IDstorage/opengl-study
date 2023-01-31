#pragma once

#include <string>
#include <optional>

class Texture {
private:
	std::string texName;

	int width;
	int height;
	int channels;

	bool hasMipmap;

	unsigned int textureObject;

	std::optional<int> targetIndex;

public:
	Texture() = default;
	Texture(const std::string& fileName, bool mipmap = true);

public:
	void Load(const std::string& file = "", bool mipmap = true);

public:
	void SetIndex(int index);

	void SetWrapOption(int sOption, int tOption);

	void SetMinifyFilter(int option);
	void SetMagnifyFilter(int option);

	void Use();
};