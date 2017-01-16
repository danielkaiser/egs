#version 330

in vec2 vf_texture_position;
out vec4 fragment_color;

uniform sampler2D rendered_texture;

void main() {
  //fragment_color = vec4(1,0,0,1);
  //if (texture(rendered_texture, vf_texture_position).xyz != vec3(1,1,1))
  fragment_color = texture(rendered_texture, vf_texture_position);
}
