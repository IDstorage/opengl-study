#version 330 core
out vec4 FragColor;

in vec4 vertexColor; // Vertex shader�� ���� ���� �Է� ���� (�̸�, Ÿ�� ��ġ)

void main() {
	FragColor = vertexColor;
}