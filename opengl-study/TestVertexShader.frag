#version 330 core
out vec4 FragColor;

in vec4 vertexColor; // Vertex shader로 부터 받은 입력 변수 (이름, 타입 일치)

void main() {
	FragColor = vertexColor;
}