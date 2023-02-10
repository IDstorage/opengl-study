#include "transform.h"

using namespace ids;

const glm::mat4 Transform::identity = glm::mat4(1.0f);

void Transform::translateTo(float x, float y, float z) {
	position.x = x;
	position.y = y;
	position.z = z;
	//transform = glm::translate(transform, glm::vec3(x, y, z));
}

void Transform::translateTo(const glm::vec3& v) {
	position = v;
	//transform = glm::translate(transform, v);
}

void Transform::translateBy(float x, float y, float z) {
	position.x += x;
	position.y += y;
	position.z += z;
}

void Transform::translateBy(const glm::vec3& v) {
	position += v;
}

void Transform::rotateTo(float x, float y, float z) {
	/*float radx = glm::radians(x), rady = glm::radians(y), radz = glm::radians(z);
	float cosx = glm::cos(radx), sinx = glm::sin(radx);
	float cosy = glm::cos(rady), siny = glm::sin(rady);
	float cosz = glm::cos(radz), sinz = glm::sin(radz);
	glm::mat4 rotx = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cosx, -sinz, 0.0f, 0.0f, sinx, cosx, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 roty = glm::mat4(cosx, 0.0f, sinx, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -sinx, 0.0f, cosx, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 rotz = glm::mat4(cosx, -sinx, 0.0f, 0.0f, sinx, cosx, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 rot = rotz * roty * rotx;

	transform = rot * transform;*/

	//transform = glm::rotate(transform, glm::radians(y), glm::vec3(0.0f, 1.0f, 0.0f));
	//transform = glm::rotate(transform, glm::radians(z), glm::vec3(0.0f, 0.0f, 1.0f));
	//transform = glm::rotate(transform, glm::radians(x), glm::vec3(1.0f, 0.0f, 0.0f));

	euler_angles.x = x;
	euler_angles.y = y;
	euler_angles.z = z;
}

void Transform::rotateTo(const glm::vec3& v) {
	euler_angles = v;
}

void Transform::rotateBy(float x, float y, float z) {
	euler_angles.x += x;
	euler_angles.y += y;
	euler_angles.z += z;
}

void Transform::rotateBy(const glm::vec3& v) {
	euler_angles += v;
}

void Transform::scaleTo(float x, float y, float z) {
	local_scale.x = x;
	local_scale.y = y;
	local_scale.z = z;
	//transform = glm::scale(transform, glm::vec3(x, y, z));
}

void Transform::scaleTo(const glm::vec3& v) {
	local_scale = v;
	//transform = glm::scale(transform, v);
}

void Transform::scaleBy(float x, float y, float z) {
	local_scale.x += x;
	local_scale.y += y;
	local_scale.z += z;
}

void Transform::scaleBy(const glm::vec3& v) {
	local_scale += v;
}


glm::vec3 Transform::getPosition() const {
	return position;
}

glm::vec3 Transform::getEulerAngles() const {
	return euler_angles;
}

glm::vec3 Transform::getLocalScale() const {
	return local_scale;
}

glm::mat4 Transform::getTransform() const {
	glm::mat4 scale = glm::scale(identity, local_scale);

	glm::mat4 roty = glm::rotate(identity, glm::radians(euler_angles.y), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotz = glm::rotate(identity, glm::radians(euler_angles.z), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 rotx = glm::rotate(identity, glm::radians(euler_angles.x), glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 tr = glm::translate(identity, position);

	return tr * rotx * rotz * roty * scale;
}