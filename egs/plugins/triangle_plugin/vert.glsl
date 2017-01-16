#version 330
in vec3 position;
in vec3 normal;

uniform mat4 view_matrix;
uniform mat4 projection;
out vec3 vf_normal;

void main() {
	vf_normal = (view_matrix*vec4(normal, 0.0)).xyz;
	gl_Position = projection*view_matrix*vec4(position, 1.0);
}
