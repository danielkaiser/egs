#define GLM_FORCE_SWIZZLE
#include "gl_context.hxx"
#include <glm/gtx/transform.hpp>
#include "glip.h"
#include <iostream>
#include <png.h>

void GLContext::rotate(glm::vec3 axis, float angle) {
  auto camera_position = ctx.get_property<glm::vec3>("camera_position");
  auto camera_center = ctx.get_property<glm::vec3>("camera_center");
  auto camera_up = ctx.get_property<glm::vec3>("camera_up");
  auto camera_forward = camera_center-camera_position;
  auto camera_right = glm::normalize(glm::cross(camera_forward, camera_up));

  axis = axis.x * camera_right + axis.y * camera_up + axis.z * camera_forward;
  camera_position = (glm::rotate(angle, axis)*glm::vec4(camera_position, 1)).xyz();
  camera_up = glm::normalize(glm::cross(camera_right, camera_center-camera_position));
  ctx.set_property("camera_position", camera_position);
  ctx.set_property("camera_center", camera_center);
  ctx.set_property("camera_up", camera_up);
  ctx.set_property("view_matrix", glm::lookAt(camera_position, camera_center, camera_up));
}

void GLContext::translate(glm::vec3 dir) {
  auto camera_position = ctx.get_property<glm::vec3>("camera_position");
  auto camera_center = ctx.get_property<glm::vec3>("camera_center");
  auto camera_up = ctx.get_property<glm::vec3>("camera_up");
  auto camera_forward = camera_center-camera_position;
  auto camera_right = glm::normalize(glm::cross(camera_forward, camera_up));
  dir = dir.x * camera_right + dir.y * camera_up + dir.z * camera_forward;
  camera_center += dir;
  camera_position += dir;
  ctx.set_property("camera_position", camera_position);
  ctx.set_property("camera_center", camera_center);
  auto view_matrix = glm::lookAt(camera_position, camera_center, camera_up);
  ctx.set_property("view_matrix", view_matrix);
}

void GLContext::zoom(float f) {
  auto camera_position = ctx.get_property<glm::vec3>("camera_position");
  auto camera_center = ctx.get_property<glm::vec3>("camera_center");
  auto camera_up = ctx.get_property<glm::vec3>("camera_up");
  camera_position = (camera_position - camera_center)*f + camera_center;
  ctx.set_property("camera_position", camera_position);
  ctx.set_property("view_matrix", glm::lookAt(camera_position, camera_center, camera_up));
}

void GLContext::set_perspective(float fovy, float aspect, float znear, float zfar) {
  ctx.set_property<glm::mat4>("projection", glm::perspective(fovy, aspect, znear, zfar));
}

GLContext::GLContext(Context &_ctx): ctx(_ctx) {
 // ctx.register_gl_context(std::shared_ptr<GLContext>(this));
}

GLContext::~GLContext() {
  //ctx.unregister_gl_context(std::shared_ptr<GLContext>(this));
}

int GLContext::draw_png(std::string filename, int width, int height) {
  int *pixels = (int *)malloc(width*height*sizeof(int));
  GLuint fb, rb, db;
  int old_viewport[4];
  glGetIntegerv(GL_VIEWPORT, old_viewport);
  assert(!glGetError());
  glViewport(0, 0, width, height);
  glGenFramebuffers(1, &fb);
  glBindFramebuffer(GL_FRAMEBUFFER, fb);
  glGenRenderbuffers(1, &rb);
  glBindRenderbuffer(GL_RENDERBUFFER, rb);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rb);
  glGenRenderbuffers(1, &db);
  glBindRenderbuffer(GL_RENDERBUFFER, db);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, db);
  glClearColor(1,1,1,1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  // need to draw scene here

  //glReadBuffer(GL_COLOR_ATTACHMENT0);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, fb);
  assert(!glGetError());
  //glReadPixels(0, 0, width, height,  GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  assert(!glGetError());
  glViewport(old_viewport[0], old_viewport[1], old_viewport[2], old_viewport[3]);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  assert(!glGetError());

  FILE *pngfp;
  int i;

  png_structp png_ptr;
  png_infop info_ptr;
  png_bytepp row_pointers;

  pngfp = fopen(filename.c_str(), "wb");
  if (!pngfp) {
    return -1;
  }

  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr) {
    fclose(pngfp);
    free(pixels);
    return -1;
  }
  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    fclose(pngfp);
    free(pixels);
    png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    return -1;
  }
  png_init_io(png_ptr, pngfp);
  png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

  row_pointers = (png_bytep*)malloc(height*sizeof(png_bytep));
  for (i = 0; i < height; i++) {
    row_pointers[i]=(png_bytep)(pixels+(height-i-1)*width);
  }
  png_set_rows(png_ptr, info_ptr, row_pointers);
  png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(pngfp);
  free(row_pointers);
  free(pixels);
  return 0;
}

void GLContext::set_property_ptr(const std::string& name, void* value, size_t size) {
  this->property_store.set_ptr(name, value, size);
}

void *GLContext::get_property_ptr(const std::string& name, void *default_val, size_t size) {
  return this->property_store.get_ptr(name, default_val, size);
}

void egs_gl_context_set_property(egs_gl_context_ref ctx, const char *name, void *data, size_t size) {
  reinterpret_cast<GLContext *>(ctx)->set_property_ptr(name, data, size);
}

void *egs_gl_context_get_property(egs_gl_context_ref ctx, const char *name, void *data, size_t size) {
  return reinterpret_cast<GLContext *>(ctx)->get_property_ptr(name, data, size);
}

egs_context_ref egs_gl_context_get_context(egs_gl_context_ref ctx) {
  return reinterpret_cast<egs_context_ref>(&reinterpret_cast<GLContext *>(ctx)->get_context());
}

void egs_display_list_element_apply(egs_display_list_elem_ref dl, egs_gl_context_ref ctx) {
  reinterpret_cast<IDisplayListElement *>(dl)->apply(*reinterpret_cast<GLContext *>(ctx));
}

void egs_display_list_element_terminate(egs_display_list_elem_ref dl) {
  reinterpret_cast<IDisplayListElement *>(dl)->~IDisplayListElement();
}

int egs_gl_context_update(egs_gl_context_ref gl_ctx, egs_display_list_ref dl_ref) {
  DisplayList *dl = reinterpret_cast<DisplayList *>(dl_ref);
  return reinterpret_cast<GLContext *>(gl_ctx)->update(std::shared_ptr<DisplayList>(dl, [=](DisplayList* dl){(void)dl;}));
}
