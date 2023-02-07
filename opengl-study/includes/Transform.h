#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ids {
	class Transform {
	private:
		glm::mat4 transform;

	public:
		void translate(float x, float y, float z);
		void translate(glm::vec3 v);

		void rotate(float x, float y, float z);
		void rotate(glm::vec3 euler);

		void scale(float x, float y, float z);
		void scale(glm::vec3 s);

	public:
		glm::vec3 getPosition() const;
		glm::vec3 getEulerAngles() const;
		glm::vec3 getLocalScale() const;
	};
}