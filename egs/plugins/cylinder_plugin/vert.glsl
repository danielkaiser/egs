#version 330
layout (location = 0) in vec3 position;
layout (location = 1) in mat4 cylinder_position;

uniform mat4 view_matrix;
uniform mat4 projection;

out vec3 vf_center_position;
out vec3 vf_dir;
out vec3 vf_position;

void main() {
  mat4 mv_cylinder_position = view_matrix * transpose(mat4(cylinder_position));
  vf_dir = normalize(mv_cylinder_position[0].xyz);
  vf_center_position = (mv_cylinder_position[3]).xyz;
  
  vf_position = (mv_cylinder_position * vec4(position, 1)).xyz;
  gl_Position = projection * vec4(vf_position, 1);
}