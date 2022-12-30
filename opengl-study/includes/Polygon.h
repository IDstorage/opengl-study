#pragma once
#include <vector>
#include <memory>

class CustomShaderProgram;

class Polygon {
private:
	std::vector<float> vertices;
	std::vector<int> indices;

	// Vertex Buffer Object / Vertex Array Object / Element array Buffer Object
	unsigned int vao, vbo, ebo;

	// Target shader program
	std::shared_ptr<CustomShaderProgram> targetShaderProg;

	bool shouldWireframe;

public:
	Polygon();
	~Polygon();

public:
	void SetVertices(const std::initializer_list<float>& verts);
	void SetIndices(const std::initializer_list<int>& ind);

	void SetAttributes(int location, int count, unsigned int type, bool normalize, int size, void* offset);

	void SetWireframeMode(bool active);

	void SetTargetShaderProg(const std::shared_ptr<CustomShaderProgram>& prog);

public:
	void Draw();
};
