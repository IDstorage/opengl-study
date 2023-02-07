#include "Polygon.h"
#include "CustomShaderProgram.h"
#include "Texture.h"
#include "Transfrom.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Polygon::Polygon() : shouldWireframe(false) {
	glGenBuffers(1, &vbo); // ������ ���� ID�� ����
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &ebo);
}
Polygon::~Polygon() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}


/*
 * ���� �����͸� vertex shader�� �����ؾ� �Ѵ�.
 *  - GPU�� ���� �����͸� ������ ���� �Ҵ�
 *  - OpenGL�� ��� �޸𸮸� �ؼ��� ������ ����
 *  - �����͸� ��� �׷��� ī�忡 ������ ������ ����
 */
void Polygon::SetVertices(const std::initializer_list<float>& verts) {
	vertices = verts;

	auto vertsArray = vertices.data();

	// vao�� VertexArray�� ���ε��ϰ� ����
	glBindVertexArray(vao);

	// GL_ARRAY_BUFFER�� vbo ���ε�
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	/*
	 * �����͸� ���� ���ε��� ���ۿ� �����ϴ� ���(�Ʊ� glBindBuffer������ GL_ARRAY_BUFFER�� vbo�� ����Ŵ)
	 * ������ ���ڴ� �׷��� ī�尡 �����͸� �����ϴ� ���
	 *  - GL_STATIC_DRAW : �����Ͱ� ���� ������ ����
	 *  - GL_DYNAMIC_DRAW : �����Ͱ� ���� �����
	 *  - GL_STREAM_DRAW : �����Ͱ� �׷��� ������ �����
	 */
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertsArray, GL_STATIC_DRAW);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	// vertex �Ӽ��� �⺻������ ��� ���ϰ� �����Ǿ� �־ glEnableVertexAttribArray�� �������. ���ڴ� location
	//glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void Polygon::SetIndices(const std::initializer_list<int>& ind) {
	indices = ind;

	auto indArray = indices.data();

	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indArray, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Polygon::SetAttributes(int location, int count, unsigned int type, bool normalize, int size, void* offset) {
	glBindVertexArray(vao);

	/*
	 * - vertices ����
	 * [   vert1   ][   vert2   ][   vert3   ]
	 * | x | y | z || x | y | z || x | y | z |
	 * 0   4   8   12   16  20  24   28  32  36
	 * <--stride-->
	 * offset: 0
	 *
	 * �� ������ �ؼ��� ����� OpenGL���� �˷���� �Ѵ�.
	 * glVertexAttribPointer�� �� ����
	 *   p1. ������ vertex �Ӽ�
	 *        - vertex shader���� �ۼ��� layout (location = 0) �ڵ尡 vertex �Ӽ��� ��ġ�� 0���� ����
	 *   p2. vertex �Ӽ��� ũ��(����) (vec3�̹Ƿ� 3)
	 *   p3. ������ Ÿ�� (OpenGL�� vec3�� GL_FLOAT��)
	 *   p4. ������ ����ȭ (GL_TRUE�� [-1, 1]�� ����, unsigned�� [0, 1]�� ����)
	 *   p5. Stride ũ��(�޸�)
	 *        - �̾��� vertices �������� �������� �� vertex���� ����
	 *   p6. ������ (void*)
	 *
	 * ���� GL_ARRAY_BUFFER�� ���ε��� ������Ʈ���� ������ ������ (�׷��� �� �Լ� ���� glBindBuffers(GL_ARRAY_BUFFER, vbo)�� ���ش�.
	 */
	glVertexAttribPointer(location, count, type, normalize ? GL_TRUE : GL_FALSE, size, offset);
	glEnableVertexAttribArray(location);

	glBindVertexArray(0);
}

void Polygon::SetWireframeMode(bool active) {
	shouldWireframe = active;
}

void Polygon::SetTargetShaderProg(const std::shared_ptr<CustomShaderProgram>& prog) {
	targetShaderProg = prog;
}


int Polygon::AddTargetTexture(const std::shared_ptr<Texture>& texture) {
	targetTextures.emplace_back(texture);
	return targetTextures.size() - 1;
}


const std::unique_ptr<Transform>& Polygon::GetTransform() const {
	return transform;
}


void Polygon::Draw() {
	if (targetShaderProg != nullptr) {
		targetShaderProg->Use();
	}

	for (auto iter = targetTextures.begin(); iter != targetTextures.end(); ++iter) {
		iter->get()->Use();
	}

	glPolygonMode(GL_FRONT_AND_BACK, shouldWireframe ? GL_LINE : GL_FILL);

	glBindVertexArray(vao);
	// glDrawArrays(GL_TRIANGLES, 0, 3);
	/*
	 * ���ε��� vao�� �������� �׸�
	 *   p1. �׸��� ���
	 *   p2. �׸����� �ϴ� ����� ����
	 *   p3. indices Ÿ��
	 *   p4. ebo ������
	 */
	glDrawElements(GL_TRIANGLES, indices.size() * sizeof(int), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}