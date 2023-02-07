#include "polygon.h"
#include "shaderprogram.h"
#include "texture.h"
#include "transform.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace ids;

Polygon::Polygon() : should_wireframe(false) {
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
 *  - �����͸� ��� �׷��� ī�忡 ������ ������ ���
 */
void Polygon::setVertices(const std::initializer_list<float>& verts) {
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

void Polygon::setIndices(const std::initializer_list<int>& ind) {
	indices = ind;

	auto indArray = indices.data();

	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indArray, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Polygon::setAttributes(int location, int count, unsigned int type, bool normalize, int size, void* offset) {
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

void Polygon::setWireframeMode(bool active) {
	should_wireframe = active;
}

void Polygon::setTargetShaderProg(const std::shared_ptr<ShaderProgram>& prog) {
	target_shaderprog = prog;
}


int Polygon::addTargetTexture(const std::shared_ptr<Texture>& texture) {
	target_textures.emplace_back(texture);
	return target_textures.size() - 1;
}


const std::unique_ptr<Transform>& Polygon::getTransform() const {
	return transform;
}


void Polygon::draw() {
	if (target_shaderprog != nullptr) {
		target_shaderprog->use();
	}

	for (auto iter = target_textures.begin(); iter != target_textures.end(); ++iter) {
		iter->get()->use();
	}

	glPolygonMode(GL_FRONT_AND_BACK, should_wireframe ? GL_LINE : GL_FILL);

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