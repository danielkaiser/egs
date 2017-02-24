#include "triangle_plugin.hxx"
#include <glm/vec4.hpp>
#include <glm/geometric.hpp>

void triangle_plugin_init_plugin(egs_context_ref ctx_ref) {
  (void) ctx_ref;
}

void triangle_plugin_terminate_plugin() {

}

TrianglePlugin::Triangle::~Triangle() {
  for (auto ctx : registered_gl_contexts) {
    delete_handler(*ctx);
  }
}

void TrianglePlugin::Triangle::apply(GLContext& ctx) {
  GLint shader_prog;
  if ((shader_prog = ctx.get_property<GLint>("Triangle::shader_program", -1)) == -1) {
    assert(!glGetError());
    auto vertex_shader = Util::compile_shader_file(GL_VERTEX_SHADER, "/triangle_plugin/vert.glsl");
    auto fragment_shader = Util::compile_shader_file(GL_FRAGMENT_SHADER, "/triangle_plugin/frag.glsl");
    shader_prog = Util::link_shader_program({vertex_shader, fragment_shader}, "fragment_color");
    ctx.set_property("Triangle::shader_program", shader_prog);
    assert(!glGetError());
  }
  glUseProgram(shader_prog);
  assert(!glGetError());
  if (!per_context_instance_data[&ctx].vao) {
    glGenBuffers(1, &per_context_instance_data[&ctx].vbo);
    glBindBuffer(GL_ARRAY_BUFFER, per_context_instance_data[&ctx].vbo);
    std::vector<glm::vec3> buf_data(2*vertices.size());
    for (size_t i=0; i<vertices.size(); i+=3) {
      buf_data[2*i+0] = vertices[i];
      buf_data[2*i+2] = vertices[i+1];
      buf_data[2*i+4] = vertices[i+2];
      if (normals.size() != vertices.size()) {
        buf_data[2*i+1] = glm::cross(vertices[i+1]-vertices[i], vertices[i+2]-vertices[i]);
        buf_data[2*i+3] = glm::cross(vertices[i+1]-vertices[i], vertices[i+2]-vertices[i]);
        buf_data[2*i+5] = glm::cross(vertices[i+1]-vertices[i], vertices[i+2]-vertices[i]);
      } else {
        buf_data[2*i+1] = normals[i];
        buf_data[2*i+3] = normals[i+1];
        buf_data[2*i+5] = normals[i+2];
      }
    }
    glBufferData(GL_ARRAY_BUFFER, buf_data.size()*sizeof(glm::vec3), &buf_data[0].x, GL_STATIC_DRAW);
    glGenVertexArrays(1, &per_context_instance_data[&ctx].vao);
    glBindVertexArray(per_context_instance_data[&ctx].vao);
    glVertexAttribPointer(glGetAttribLocation(shader_prog, "position"), 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (void*)(0*sizeof(GLfloat)));
    glVertexAttribPointer(glGetAttribLocation(shader_prog, "normal"), 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    assert(!glGetError());
    egs_printf(EGS_DEBUG, "created triangle buffer\n");
  }
  glBindVertexArray(per_context_instance_data[&ctx].vao);
  egs_directional_light light = ctx.get_context().get_property<egs_directional_light>("light::directional_light");
  auto light_dir = glm::vec3(light.direction_x, light.direction_y, light.direction_z);
  glUniform3fv(glGetUniformLocation(shader_prog, "light_direction"), 1, &light_dir.x);
  glUniform1f(glGetUniformLocation(shader_prog, "ambient"), light.ambient);
  glUniform1f(glGetUniformLocation(shader_prog, "diffuse"), light.diffuse);
  glUniform1f(glGetUniformLocation(shader_prog, "specular"), light.specular);
  glUniformMatrix4fv(glGetUniformLocation(shader_prog, "view_matrix"), 1, GL_FALSE, &ctx.get_context().get_property<glm::mat4>("view_matrix")[0].x);
  glUniformMatrix4fv(glGetUniformLocation(shader_prog, "projection"), 1, GL_FALSE, &ctx.get_context().get_property<glm::mat4>("projection")[0].x);
  glUniform3f(glGetUniformLocation(shader_prog, "color"), ((color&0x00ff0000)>>16)/255., ((color&0x0000ff00)>>8)/255., ((color&0x000000ff)/255.));
  glDisable(GL_CULL_FACE);
  glDrawArrays(GL_TRIANGLES, 0, (GLint)vertices.size());
  glEnable(GL_CULL_FACE);
  assert(!glGetError());
}

void TrianglePlugin::Triangle::delete_handler(GLContext& ctx) {
  egs_printf(EGS_DEBUG, "triangle delete handler called\n");
  glDeleteBuffers(1, &per_context_instance_data[&ctx].vbo);
  glDeleteVertexArrays(1, &per_context_instance_data[&ctx].vao);
  egs_printf(EGS_DEBUG, "triangle deleted\n");
}

egs_display_list_elem_ref triangle_plugin_create_triangle(egs_context_ref ctx, int n_points, float *vertices, float* normals, long color=0) {
  (void) ctx;
  assert((n_points%3) == 0);
  std::vector<glm::vec3> vert;
  for (int i=0; i<n_points; i++) {
    vert.push_back(glm::vec3(vertices[3*i], vertices[3*i+1], vertices[3*i+2]));
  }
  if (normals) {
    std::vector<glm::vec3> norm;
    for (int i=0; i<n_points; i++) {
      norm.push_back(glm::vec3(normals[3*i], normals[3*i+1], normals[3*i+2]));
    }
    return reinterpret_cast<egs_display_list_elem_ref>(new TrianglePlugin::Triangle(vert, norm, color));
  } else {
    return reinterpret_cast<egs_display_list_elem_ref>(new TrianglePlugin::Triangle(vert, color));
  }
}
