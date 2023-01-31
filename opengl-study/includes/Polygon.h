#pragma once
#include <vector>
#include <memory>

class CustomShaderProgram;
class Texture;

class Polygon {
private:
	std::vector<float> vertices;
	std::vector<int> indices;

	// Vertex Buffer Object / Vertex Array Object / Element array Buffer Object
	unsigned int vao, vbo, ebo;

	// Target shader program
	std::shared_ptr<CustomShaderProgram> targetShaderProg;

	// Target texture
	std::vector<std::shared_ptr<Texture>> targetTextures;

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
	
	int AddTargetTexture(const std::shared_ptr<Texture>& texture);

public:
	void Draw();
};
