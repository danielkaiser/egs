#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture_position;
layout (location = 2) in vec4 sphere_position;

uniform mat4 view_matrix;
uniform mat4 projection;

out vec2 vf_texture_position;

void main() {
  vf_texture_position = texture_position;
  gl_Position = projection * view_matrix * vec4(sphere_position.w * position, 1.0);
}
