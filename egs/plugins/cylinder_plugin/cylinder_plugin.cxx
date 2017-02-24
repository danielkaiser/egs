#include "cylinder_plugin.hxx"
#include <glm/vec4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/transform.hpp>
#include "egs.h"

#define CYLINDER_VERTICES 16

void cylinder_plugin_init_plugin(egs_context_ref ctx_ref) {
  (void) ctx_ref;
}

void cylinder_plugin_terminate_plugin() {

}

void create_cylinder(GLuint vertex_buffer, int n) {
  GLfloat *positions = (GLfloat *)malloc(2 * 3 * n * sizeof(GLfloat));
  int i;
  for (i = 0; i < n; i ++) {
    positions[6 * i + 0] = 0.5;
    positions[6 * i + 3] = -0.5;
    positions[6 * i + 1] = cos(i * 2 * M_PI / (n-1));
    positions[6 * i + 2] = sin(i * 2 * M_PI / (n-1));
    positions[6 * i + 4] = cos(i * 2 * M_PI / (n-1));
    positions[6 * i + 5] = sin(i * 2 * M_PI / (n-1));
  }
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, 2 * 3 * n * sizeof(GLfloat), positions, GL_STATIC_DRAW);
  free(positions);
}

CylinderPlugin::Cylinder::Cylinder(const std::vector<glm::vec3> &start, const std::vector<glm::vec3> &end, const std::vector<float> &radii, long color):
start_vec(start), end_vec(end), radii_vec(radii), color(color){}

CylinderPlugin::Cylinder::~Cylinder() {
  for (auto ctx : registered_gl_contexts) {
    delete_handler(*ctx);
  }
}

void CylinderPlugin::Cylinder::apply(GLContext& ctx) {
  GLint shader_prog;
  if ((shader_prog = ctx.get_property<int>("cylinder::shader_program", -1)) == -1) {
    GLuint compiled_shader[2];
    compiled_shader[0] = egs_util_compile_shader_file(GL_VERTEX_SHADER, "/cylinder_plugin/vert.glsl");
    compiled_shader[1] = egs_util_compile_shader_file(GL_FRAGMENT_SHADER, "/cylinder_plugin/frag.glsl");
    shader_prog = egs_util_link_shader_program(2, compiled_shader, "fragment_color");
    ctx.set_property<int>("cylinder::shader_program", shader_prog);
  }
  glGetError();
  glUseProgram(shader_prog);
  if (!per_context_instance_data[&ctx].vao) {
    assert(glGetError() == 0);
    glGenBuffers(1, &per_context_instance_data[&ctx].vbo);
    glGenVertexArrays(1, &per_context_instance_data[&ctx].vao);
    glBindVertexArray(per_context_instance_data[&ctx].vao);
    glBindBuffer(GL_ARRAY_BUFFER, per_context_instance_data[&ctx].vbo);

    assert(glGetError() == 0);
    glVertexAttribPointer(glGetAttribLocation(shader_prog, "position"), 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    assert(glGetError() == 0);

    create_cylinder(per_context_instance_data[&ctx].vbo, CYLINDER_VERTICES/2.);

    std::vector<glm::mat4> cylinder_instances;
    for (size_t i = 0; i < start_vec.size(); i++) {
      glm::vec3 dir_vec = start_vec[i] - end_vec[i];
      glm::vec3 normalized_d = 1 / glm::length(dir_vec) * dir_vec;
      glm::vec3 axis = {0, -normalized_d.z, normalized_d.y};
      float angle = acos(normalized_d.x);
      glm::vec3 scale = {glm::length(dir_vec), radii_vec[i], radii_vec[i]};
      glm::mat4 transform;
      if (glm::length(axis) <= 10e-5) {
        transform = glm::translate(start_vec[i] - 0.5f*(dir_vec)) * glm::scale(scale);
      } else {
        transform = glm::translate(start_vec[i] - 0.5f*(dir_vec)) * glm::rotate(angle, axis) * glm::scale(scale);
      }
      cylinder_instances.push_back(glm::transpose(transform));
    }

    glGenBuffers(1, &per_context_instance_data[&ctx].instance_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, per_context_instance_data[&ctx].instance_buffer);

    GLuint cylinder_location = glGetAttribLocation(shader_prog, "cylinder_position");
    for (int i=0; i<3; i++) {
      glEnableVertexAttribArray(cylinder_location+i);
      glVertexAttribPointer(cylinder_location+i, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 16, (void*)(sizeof(float) * 4 * i));
      glVertexAttribDivisor(cylinder_location+i, 1);
    }
    assert(glGetError() == 0);
    glBufferData(GL_ARRAY_BUFFER, start_vec.size()*sizeof(glm::mat4), (void*)cylinder_instances.data(), GL_STATIC_DRAW);
    assert(glGetError() == 0);

    egs_printf(EGS_DEBUG, "created cylinder buffer\n");
  }
  glBindVertexArray(per_context_instance_data[&ctx].vao);
  egs_directional_light_t light = ctx.get_context().get_property<egs_directional_light_t>("light::directional_light");
  glUniform3f(glGetUniformLocation(shader_prog, "light_direction"), light.direction_x, light.direction_y, light.direction_z);
  glUniform1f(glGetUniformLocation(shader_prog, "ambient"), light.ambient);
  glUniform1f(glGetUniformLocation(shader_prog, "diffuse"), light.diffuse);
  glUniform1f(glGetUniformLocation(shader_prog, "specular"), light.specular);
  glUniform3f(glGetUniformLocation(shader_prog, "color"), ((color&0x00ff0000)>>16)/255., ((color&0x0000ff00)>>8)/255., ((color&0x000000ff)/255.));
  glUniformMatrix4fv(glGetUniformLocation(shader_prog, "view_matrix"), 1, GL_FALSE, &ctx.get_property<glm::mat4>("view_matrix")[0].x);
  glUniformMatrix4fv(glGetUniformLocation(shader_prog, "projection"), 1, GL_FALSE, &ctx.get_property<glm::mat4>("projection")[0].x);

  assert(glGetError() == 0);
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, CYLINDER_VERTICES, (GLint)start_vec.size());
  assert(glGetError() == 0);
}

void CylinderPlugin::Cylinder::delete_handler(GLContext& ctx) {
  egs_printf(EGS_DEBUG, "cylinder delete handler called\n");
  const GLuint buffers[2] = {per_context_instance_data[&ctx].vbo, per_context_instance_data[&ctx].instance_buffer};
  glDeleteBuffers(2, buffers);
  glDeleteVertexArrays(1, &per_context_instance_data[&ctx].vao);
  egs_printf(EGS_DEBUG, "cylinder deleted\n");
}

egs_display_list_elem_ref cylinder_plugin_create_cylinder(egs_context_ref ctx, int n, float *start, float* end, float *radii, long color=0) {
  (void) ctx;
  std::vector<glm::vec3> start_vec;
  std::vector<glm::vec3> end_vec;
  std::vector<float> radii_vec;
  for (int i=0; i<n; i++) {
    start_vec.push_back(glm::vec3(start[3*i], start[3*i+1], start[3*i+2]));
    end_vec.push_back(glm::vec3(end[3*i], end[3*i+1], end[3*i+2]));
    radii_vec.push_back(radii[i]);
  }
  return reinterpret_cast<egs_display_list_elem_ref>(new CylinderPlugin::Cylinder(start_vec, end_vec, radii_vec, color));
}
