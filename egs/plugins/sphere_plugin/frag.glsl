#version 330

layout(location = 0) out vec4 fragment_color;

/*<<<<<<< HEAD
in vec2 vf_texture_position;

uniform sampler2D rendered_texture;

void main() {  
  fragment_color = texture(rendered_texture, vf_texture_position);
=======*/
//uniform vec3 ambient;
uniform vec3 color;
uniform vec3 light_direction;
//uniform float shininess;
//uniform float strength;

in vec3 vf_center_position;
in vec3 vf_position;
in vec3 vf_sphere_color;
void main() {
  float strength = 0.5;
  float shininess = 51.2;
  vec3 n = normalize(vf_position-vf_center_position);

  float diffuse = max(0.0, dot(n, -light_direction));

  vec3 half_vector = normalize(normalize(-light_direction) - normalize(vf_position) );
  float specular_angle = max(dot(half_vector, n), 0.0);
  float specular = pow(specular_angle, shininess);

  vec3 scattered_light = vec3(0.6) * diffuse;
  vec3 reflected_light = vec3(0.4) * specular * strength;
  fragment_color = vec4(min(vf_sphere_color * 0.2 + vf_sphere_color * scattered_light + reflected_light, vec3(1.0)), 1.0);
//>>>>>>> develop
}
