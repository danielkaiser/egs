#version 330

layout(location = 0) out vec3 fragment_color;

void main() {
	//float diffuse_light = max(0.0, dot(-normalize(vf_normal), normalize(light_direction)));
  fragment_color = vec3(0,0,0);
}
