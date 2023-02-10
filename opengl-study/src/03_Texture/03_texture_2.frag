#version 330 core
out vec4 FragColor;

in vec3 VertexColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float threshold;

void main() {
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), threshold);// * vec4(VertexColor, 1.0f);
}