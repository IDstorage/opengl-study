#pragma once
#include <vector>
#include <memory>

namespace ids {
	class ShaderProgram;
	class Texture;
	class Transform;

	class Polygon {
	private:
		std::vector<float> vertices;
		std::vector<int> indices;

		// Vertex Buffer Object / Vertex Array Object / Element array Buffer Object
		unsigned int vao, vbo, ebo;

		// Target shader program
		std::shared_ptr<ShaderProgram> target_shaderprog;

		// Target texture
		std::vector<std::shared_ptr<Texture>> target_textures;

		bool should_wireframe;

	private:
		std::unique_ptr<Transform> transform;

	public:
		Polygon();
		~Polygon();

	public:
		void setVertices(const std::initializer_list<float>& verts);
		void setIndices(const std::initializer_list<int>& ind);

		void setAttributes(int location, int count, unsigned int type, bool normalize, int size, void* offset);

		void setWireframeMode(bool active);

		void setTargetShaderProg(const std::shared_ptr<ShaderProgram>& prog);

		int addTargetTexture(const std::shared_ptr<Texture>& texture);

	public:
		const std::unique_ptr<Transform>& getTransform() const;

	public:
		void draw();
	};
}