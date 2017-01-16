#version 330

layout(location = 0) out vec4 fragment_color;
in vec2 vf_texture_position;

uniform sampler2D rendered_texture;

void main() {
  fragment_color = texture(rendered_texture, vf_texture_position);
}
