#pragma once

#include "glm_required.h"

namespace ids {
	class Transform {
	private:
		glm::mat4 transform;

	public:
		static const glm::mat4 identity;

	public:
		void translate(float x, float y, float z);
		void translate(const glm::vec3& v);

		void rotate(float x, float y, float z);
		void rotate(const glm::vec3& euler);

		void scale(float x, float y, float z);
		void scale(const glm::vec3& s);

	public:
		glm::vec3 getPosition() const;
		glm::vec3 getEulerAngles() const;
		glm::vec3 getLocalScale() const;
	};
}