#pragma once

#include "glm_required.h"

namespace ids {
	class Transform {
	private:
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 euler_angles = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 local_scale = glm::vec3(1.0f, 1.0f, 1.0f);

	public:
		static const glm::mat4 identity;

	public:
		void translateTo(float x, float y, float z);
		void translateTo(const glm::vec3& v);
		void translateBy(float x, float y, float z);
		void translateBy(const glm::vec3& v);

		void rotateTo(float x, float y, float z);
		void rotateTo(const glm::vec3& euler);
		void rotateBy(float x, float y, float z);
		void rotateBy(const glm::vec3& euler);

		void scaleTo(float x, float y, float z);
		void scaleTo(const glm::vec3& s);
		void scaleBy(float x, float y, float z);
		void scaleBy(const glm::vec3& s);

	public:
		glm::vec3 getPosition() const;
		glm::vec3 getEulerAngles() const;
		glm::vec3 getLocalScale() const;

		glm::mat4 getTransform() const;
	};
}