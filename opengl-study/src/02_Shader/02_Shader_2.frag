#version 330 core
out vec4 FragColor;

// OpenGL 코드에서 이 값을 수정
uniform vec4 customColor;

void main() {
	FragColor = customColor;
}