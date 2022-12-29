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
	/*
	 * 데이터를 현재 바인딩된 버퍼에 복사하는 기능(아까 glBindBuffer때문에 GL_ARRAY_BUFFER는 vbo를 가리킴)
	 * 마지막 인자는 그래픽 카드가 데이터를 관리하는 방법
	 *  - GL_STATIC_DRAW : 데이터가 거의 변하지 않음
	 *  - GL_DYNAMIC_DRAW : 데이터가 자주 변경됨
	 *  - GL_STREAM_DRAW : 데이터가 그려질 때마다 변경됨
	 */
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertsArray, GL_STATIC_DRAW);

	/*
	 * - vertices 구조
	 * [   vert1   ][   vert2   ][   vert3   ]
	 * | x | y | z || x | y | z || x | y | z |
	 * 0   4   8   12   16  20  24   28  32  36
	 * <--stride-->
	 * offset: 0
	 * 
	 * 이 구조를 해석할 방법을 OpenGL에게 알려줘야 한다.
	 * glVertexAttribPointer가 그 역할
	 *   p1. 설정할 vertex 속성
	 *        - vertex shader에서 작성한 layout (location = 0) 코드가 vertex 속성의 위치를 0으로 지정
	 *   p2. vertex 속성의 크기(개수) (vec3이므로 3)
	 *   p3. 데이터 타입 (OpenGL의 vec3는 GL_FLOAT형)
	 *   p4. 데이터 정규화 (GL_TRUE면 [-1, 1]로 매핑, unsigned면 [0, 1]로 매핑)
	 *   p5. Stride 크기(메모리)
	 *        - 이어진 vertices 구조에서 어디까지가 한 vertex인지 구분
	 *   p6. 오프셋 (void*)
	 * 
	 * 현재 GL_ARRAY_BUFFER에 바인딩된 오브젝트에서 데이터 가져옴 (그래서 이 함수 전에 glBindBuffers(GL_ARRAY_BUFFER, vbo)를 해준다.
	 */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	// vertex 속성은 기본적으로 사용 못하게 설정되어 있어서 glEnableVertexAttribArray를 해줘야함. 인자는 location
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

void StaticPolygon::SetIndices(const std::initializer_list<int>& ind) {
	indices = ind;

	auto indArray = indices.data();

	glBindVertexArray(vao);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indArray, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void StaticPolygon::SetWireframeMode(bool active) {
	shouldWireframe = active;
}


void StaticPolygon::Draw() {
	glPolygonMode(GL_FRONT_AND_BACK, shouldWireframe ? GL_LINE : GL_FILL);

	glBindVertexArray(vao);
	// glDrawArrays(GL_TRIANGLES, 0, 3);
	/*
	 * 바인딩된 vao를 바탕으로 그림
	 *   p1. 그리기 모드
	 *   p2. 그리고자 하는 요소의 개수
	 *   p3. indices 타입
	 *   p4. ebo 오프셋
	 */
	glDrawElements(GL_TRIANGLES, indices.size() * sizeof(int), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}