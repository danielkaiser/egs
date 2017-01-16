#version 330
in vec3 position;
in vec2 texture_position;

uniform mat4 view_matrix;
uniform mat4 projection;
out vec2 vf_texture_position;

void main() {
  vf_texture_position = texture_position;
  gl_Position = projection*view_matrix*vec4(position, 1.0);
}
