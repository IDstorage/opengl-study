#version 330 core
out vec4 FragColor;

// OpenGL �ڵ忡�� �� ���� ����
uniform vec4 customColor;

void main() {
	FragColor = customColor;
}