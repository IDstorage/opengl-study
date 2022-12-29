#pragma once
#include <vector>

class StaticPolygon {
private:
	std::vector<float> vertices;
	std::vector<int> indices;

	// Vertex Buffer Object / Vertex Array Object / Element array Buffer Object
	unsigned int vao, vbo, ebo;

	bool shouldWireframe;

public:
	StaticPolygon();
	~StaticPolygon();

public:
	void SetVertices(const std::initializer_list<float>& verts);
	void SetIndices(const std::initializer_list<int>& ind);

	void SetWireframeMode(bool active);

public:
	void Draw();
};
