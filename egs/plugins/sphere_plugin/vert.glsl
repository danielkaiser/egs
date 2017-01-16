#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 sphere_position;
layout (location = 2) in vec3 sphere_color;

uniform mat4 view_matrix;
uniform mat4 projection;

out vec3 vf_center_position;
out vec3 vf_position;
out vec3 vf_sphere_color;

void main() {
  vf_sphere_color = sphere_color;
  vf_center_position = (view_matrix * vec4(sphere_position.xyz, 1.0)).xyz;
  vf_position = vf_center_position + sphere_position.w * position;
  gl_Position = projection * vec4(vf_position, 1.0);
}


/*#version 330

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
*/
