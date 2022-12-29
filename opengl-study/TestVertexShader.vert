#version 330 core

// vertex attribute ��ġ�� 0
layout (location = 0) in vec3 pos;

// fragment shader�� ���� �÷� ���
out vec4 vertexColor;

void main() {
	gl_Position = vec4(pos, 1.0);
	vertexColor = vec4(0.5f, 0.0f, 0.0f, 1.0f);
}