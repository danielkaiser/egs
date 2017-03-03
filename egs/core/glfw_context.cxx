//
//  glfw_context.cpp
//  egs
//
//  Created by Daniel Kaiser on 15/06/16.
//  Copyright © 2016 Daniel Kaiser. All rights reserved.
//

#ifndef NO_GLFW_CONTEXT

#include "glfw_context.hxx"
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <memory>
#include <unistd.h>
#include "context.hxx"

unsigned int GLFWContext::window_count = 0;

GLFWContext::GLFWContext(Context &ctx) : GLFWContext(ctx, 600, 600) {}

GLFWContext::GLFWContext(Context &ctx, int width, int height, int pos_x, int pos_y) : GLFWContext(ctx, width, height) {
  glfwSetWindowPos(window, pos_x, pos_y);
}

GLFWContext::GLFWContext(Context &ctx, int width, int height) : GLContext(ctx), GLInteractivityMixin(static_cast<GLContext &>(*this)) {
  char workind_dir[1000];
  getcwd(workind_dir, 1000);
  if (!glfwInit()) {
    std::cerr << "Error while glfwinit" << std::endl;
  }
  chdir(workind_dir);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_SAMPLES, 4);

  window = glfwCreateWindow(width, height, "EGS", 0, 0);
  window_count++;

  if (!window) {
    egs_printf(EGS_ERROR, "Error while creating GLFW window.\n");
    return;
  }
  glfwMakeContextCurrent(window);

  glip_context_t *glip_ctx = glip_init_gl();
  assert(glip_ctx);
  glip_set_current_context(glip_ctx);

  glfwSetWindowUserPointer(window, this);
  glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
    auto ctx_ptr = static_cast<GLFWContext*>(glfwGetWindowUserPointer(window));
    ctx_ptr->key_callback(*ctx_ptr, key, action);
  });
  glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos) {
    auto ctx_ptr = static_cast<GLFWContext*>(glfwGetWindowUserPointer(window));
    int mouse_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);
    ctx_ptr->mouse_callback(*ctx_ptr, xpos, ypos, mouse_state);
  });

  glfwSetScrollCallback(window, [](GLFWwindow *window, double xoffset, double yoffset) {
    auto ctx_ptr = static_cast<GLFWContext*>(glfwGetWindowUserPointer(window));
    ctx_ptr->scroll_callback(*ctx_ptr, xoffset, yoffset);
  });

  glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
    auto ctx_ptr = static_cast<GLFWContext*>(glfwGetWindowUserPointer(window));
    glfwMakeContextCurrent(window);
    ctx_ptr->resize_callback(*ctx_ptr, width, height);
  });

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}

bool GLFWContext::update(std::shared_ptr<DisplayList> display_list) {
  glfwMakeContextCurrent(window);
  glm::vec4 clear_color = get_property<glm::vec4>("clear_color", glm::vec4(1, 1, 1, 1));
  glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  assert(glGetError() == 0);

  display_list->apply(*this);

  glfwSwapBuffers(window);
  glfwPollEvents();

  return !glfwWindowShouldClose(window);
}

GLFWContext::~GLFWContext() {
  if (window) {
    glfwDestroyWindow(window);
  }
  if ((--window_count) == 0) {
    glfwTerminate();
  }
}

int GLFWContext::get_width() {
  int width, height;
  glfwGetWindowSize(window, &width, &height);
  return width;
}

int GLFWContext::get_height() {
  int width, height;
  glfwGetWindowSize(window, &width, &height);
  return height;
}

egs_glfw_context_ref egs_glfw_context_create(egs_context_ref ctx_ref) {
  Context *ctx = reinterpret_cast<Context *>(ctx_ref);
  GLFWContext *glfw_ctx = new GLFWContext(*ctx);
  return reinterpret_cast<egs_glfw_context_ref>(glfw_ctx);
}

void egs_glfw_context_destroy(egs_glfw_context_ref glfw_ctx) {
  delete reinterpret_cast<GLFWContext *>(glfw_ctx);
}

#endif
