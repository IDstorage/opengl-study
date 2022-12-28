#include "StaticPolygon.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

StaticPolygon::StaticPolygon() : shouldWireframe(false) {
	glGenBuffers(1, &vbo); // 고유한 버퍼 ID를 생성
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &ebo);
}
StaticPolygon::~StaticPolygon() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
}


/*
 * 정점 데이터를 vertex shader에 전달해야 한다.
 *  - GPU에 정점 데이터를 저장할 공간 할당
 *  - OpenGL이 어떻게 메모리를 해석할 것인지 구성
 *  - 데이터를 어떻게 그래픽 카드에 전달할 것인지 명시
 */
void StaticPolygon::SetVertices(const std::initializer_list<float>& verts) {
	vertices = verts;

	auto vertsArray = vertices.data();

	// vao를 VertexArray에 바인딩하고 시작
	glBindVertexArray(vao);

	// GL_ARRAY_BUFFER에 vbo 바인드
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	/* 데이터를 현재 바인딩된 버퍼에 복사하는 기능(아까 glBindBuffer때문에 GL_ARRAY_BUFFER는 vbo를 가리킴)
	 * 마지막 인자는 그래픽 카드가 데이터를 관리하는 방법
	 *  - GL_STATIC_DRAW : 데이터가 거의 변하지 않음
	 *  - GL_DYNAMIC_DRAW : 데이터가 자주 변경됨
	 *  - GL_STREAM_DRAW : 데이터가 그려질 때마다 변경됨
	 */
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertsArray, GL_STATIC_DRAW);
}

void StaticPolygon::SetIndices(const std::initializer_list<int>& ind) {
	indices = ind;

	auto indArray = indices.data();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indArray, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
	glEnableVertexAttribArray(0);
}

void StaticPolygon::SetWireframeMode(bool active) {
	shouldWireframe = active;
}


void StaticPolygon::Draw() {
	glPolygonMode(GL_FRONT_AND_BACK, shouldWireframe ? GL_LINE : GL_FILL);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size() * sizeof(int), GL_UNSIGNED_INT, 0);
}