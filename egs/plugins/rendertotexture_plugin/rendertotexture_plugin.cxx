#include "rendertotexture_plugin.hxx"
#include <glm/vec4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/transform.hpp>

void plot_plugin_init_plugin(egs_context_ref ctx_ref) {
  (void) ctx_ref;
}

void plot_plugin_terminate_plugin() {

}

RenderToTexturePlugin::Plot::~Plot() {
  glDeleteFramebuffers(1, &framebuffer);
  glDeleteTextures(1, &render_texture);
  egs_printf(EGS_DEBUG, "plot destructor\n");
}

void RenderToTexturePlugin::Plot::apply(GLContext& ctx) {
  assert(!glGetError());
  int old_framebuffer;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &old_framebuffer);
  assert(!glGetError());
  if (!framebuffer) {
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glGenTextures(1, &render_texture);
    glBindTexture(GL_TEXTURE_2D, render_texture);
    assert(!glGetError());
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, render_texture, 0);
    assert(!glGetError());
    GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, draw_buffers);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    assert(!glGetError());
    egs_printf(EGS_DEBUG, "plot plugin: created framebuffer %d and texture %d\n", framebuffer, render_texture);
  }
  assert(!glGetError());
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  glEnable(GL_LINE_SMOOTH);
  glClear(GL_COLOR_BUFFER_BIT);
  int old_viewport[4];
  glGetIntegerv(GL_VIEWPORT, old_viewport);
  glViewport(0, 0, width, height);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, render_texture);
  egs_printf(EGS_DEBUG, "plot plugin: framebuffer %d and texture %d bound, applying displaylist\n", framebuffer, render_texture);

  display_list->apply(ctx);

  glViewport(old_viewport[0], old_viewport[1], old_viewport[2], old_viewport[3]);
  glBindFramebuffer(GL_FRAMEBUFFER, old_framebuffer);
  if (enable_mipmapping) {
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  assert(!glGetError());
  egs_printf(EGS_DEBUG, "plot plugin: restored framebuffer %d and viewport\n", framebuffer);
  assert(!glGetError());
}

/*********************************************************************************/

RenderToTexturePlugin::Surface::Surface(glm::vec3 position, glm::vec3 width, glm::vec3 height) {
  if (glm::dot(width, height) > 10e-4) {
    egs_printf(EGS_WARNING, "Surface: width and height are not in right angle.\n");
  }
  pos1 = position;
  pos2 = position + width;
  pos3 = position + width + height;
  pos4 = position + height;
}

RenderToTexturePlugin::Surface::~Surface() {
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
  egs_printf(EGS_DEBUG, "surface destructor\n");
}

void RenderToTexturePlugin::Surface::apply(GLContext& ctx) {
  GLint shader_prog;
  if ((shader_prog = ctx.get_property<GLint>("Surface::shader_program", -1)) == -1) {
    auto vertex_shader = Util::compile_shader_file(GL_VERTEX_SHADER, "/plot_plugin/surf_vert.glsl");
    auto fragment_shader = Util::compile_shader_file(GL_FRAGMENT_SHADER, "/plot_plugin/surf_frag.glsl");
    shader_prog = Util::link_shader_program({vertex_shader, fragment_shader}, "fragment_color");
    ctx.set_property("Surface::shader_program", shader_prog);
  }
  glUseProgram(shader_prog);
  if (!vbo) {
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    std::vector<float> buf_data = {
      pos1.x, pos1.y, pos1.z, 0, 0,
      pos2.x, pos2.y, pos2.z, 1, 0,
      pos3.x, pos3.y, pos3.z, 1, 1,
      pos2.x, pos2.y, pos2.z, 1, 0,
      pos3.x, pos3.y, pos3.z, 1, 1,
      pos4.x, pos4.y, pos4.z, 0, 1
    };
    glBufferData(GL_ARRAY_BUFFER, buf_data.size()*sizeof(float), buf_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(glGetAttribLocation(shader_prog, "position"), 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void*)(0*sizeof(GLfloat)));
    glVertexAttribPointer(glGetAttribLocation(shader_prog, "texture_position"), 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    egs_printf(EGS_DEBUG, "created surface buffer\n");
  }
  glBindVertexArray(vao);

  glDisable(GL_CULL_FACE);
  glUniform1i(glGetUniformLocation(shader_prog, "rendered_texture"), 0);
  glUniformMatrix4fv(glGetUniformLocation(shader_prog, "view_matrix"), 1, GL_FALSE, &ctx.get_context().get_property<glm::mat4>("view_matrix")[0].x);
  glUniformMatrix4fv(glGetUniformLocation(shader_prog, "projection"), 1, GL_FALSE, &ctx.get_context().get_property<glm::mat4>("projection")[0].x);
  glDrawArrays(GL_TRIANGLES, 0, 2*3);
  glEnable(GL_CULL_FACE);
}

egs_display_list_elem_ref plot_plugin_create_surface(egs_context_ref ctx, float *positions) {
  (void) ctx;
  std::vector<glm::vec3> pos;
  for (int i=0; i<4; i++) {
    pos.push_back(glm::vec3(positions[3*i], positions[3*i+1], positions[3*i+2]));
  }
  return reinterpret_cast<egs_display_list_elem_ref>(new RenderToTexturePlugin::Surface(pos[0], pos[1], pos[2]));
}
