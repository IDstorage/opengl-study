#include "transform.h"

using namespace ids;

const glm::mat4 Transform::identity = glm::mat4(1.0f);

void Transform::translate(float x, float y, float z) {
	transform = glm::translate(transform, glm::vec3(x, y, z));
}

void Transform::translate(const glm::vec3& v) {
	transform = glm::translate(transform, v);
}

void Transform::rotate(float x, float y, float z) {
	/*float radx = glm::radians(x), rady = glm::radians(y), radz = glm::radians(z);
	float cosx = glm::cos(radx), sinx = glm::sin(radx);
	float cosy = glm::cos(rady), siny = glm::sin(rady);
	float cosz = glm::cos(radz), sinz = glm::sin(radz);
	glm::mat4 rotx = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cosx, -sinz, 0.0f, 0.0f, sinx, cosx, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 roty = glm::mat4(cosx, 0.0f, sinx, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -sinx, 0.0f, cosx, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 rotz = glm::mat4(cosx, -sinx, 0.0f, 0.0f, sinx, cosx, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 rot = rotz * roty * rotx;

	transform = rot * transform;*/

	transform = glm::rotate(transform, glm::radians(y), glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(z), glm::vec3(0.0f, 0.0f, 1.0f));
	transform = glm::rotate(transform, glm::radians(x), glm::vec3(1.0f, 0.0f, 0.0f));
}

void Transform::rotate(const glm::vec3& v) {
	rotate(v.x, v.y, v.z);
}

void Transform::scale(float x, float y, float z) {
	transform = glm::scale(transform, glm::vec3(x, y, z));
}

void Transform::scale(const glm::vec3& v) {
	transform = glm::scale(transform, v);
}