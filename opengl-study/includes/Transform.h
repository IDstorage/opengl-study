#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Transform {
private:
	glm::vec4 position;
	glm::vec4 eulerAngles;
	glm::vec4 scale;

public:
	void Translate(float x, float y, float z);
	void Translate(glm::vec3 v);

	void RotateEuler(float x, float y, float z);
	void RotateEuler(glm::vec3 euler);

	void Scale(float x, float y, float z);
	void Scale(glm::vec3 s);
};