#version 330

in vec3 vf_normal;
out vec4 fragment_color;

//uniform vec4 light_color;
uniform vec3 light_direction;
uniform float ambient;
uniform float diffuse;
uniform float specular;

uniform vec3 color;

void main() {
	float diffuse_light = max(0.0, abs(dot(-normalize(vf_normal), normalize(light_direction))));
  fragment_color = vec4(max(min(ambient * color + diffuse * diffuse_light * color, 1.0), 0.0), 1.0);
}
