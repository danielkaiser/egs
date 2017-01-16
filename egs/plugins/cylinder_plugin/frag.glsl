#version 330

out vec4 fragment_color;

uniform vec3 color;
uniform vec3 light_direction;
uniform float ambient;
uniform float diffuse;
uniform float specular;

in vec3 vf_center_position;
in vec3 vf_position;
in vec3 vf_dir;

void main() {
  vec3 n = normalize(vf_position - vf_center_position - vf_dir * dot(vf_dir, vf_position-vf_center_position));
  
  float diffuse_light = max(0.0, dot(-n, light_direction));
  
  //vec3 half_vector = normalize( normalize(light_direction) - normalize(vf_position) );
  //float specular_angle = max(dot(half_vector, n), 0.0);
  //float specular = pow(specular_angle, shininess);
  
  //vec3 scattered_light = vec3(1.0) * diffuse;
  //vec3 reflected_light = vec3(1.0) * specular * strength;
  
  fragment_color = vec4(max(min(ambient * color + diffuse * diffuse_light * color, 1.0), 0.0), 1.0);
}