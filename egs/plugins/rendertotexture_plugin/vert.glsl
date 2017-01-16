#version 330
in vec2 position;

uniform mat4 model_matrix;

void main() {
	gl_Position = model_matrix * vec4(position, 0.0, 1.0);
}
