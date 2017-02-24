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

unsigned int GLFWContext::window_count = 0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  (void) scancode;
  (void) mods;
  GLFWContext *ctx = static_cast<GLFWContext*>(glfwGetWindowUserPointer(window));
  if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    ctx->rotate(glm::vec3(0,1,0), 0.1);
  } else if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    ctx->rotate(glm::vec3(0,1,0), -0.1);
  } else if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    ctx->rotate(glm::vec3(1,0,0), 0.1);
  } else if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    ctx->rotate(glm::vec3(1,0,0), -0.1);
  }

  if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    ctx->translate(glm::vec3(0,1,0));
  } else if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    ctx->translate(glm::vec3(0, -1,0));
  } else if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    ctx->translate(glm::vec3(-1,0,0));
  } else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    ctx->translate(glm::vec3(1,0,0));
  } else if (key == GLFW_KEY_T && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tstruct);
    ctx->draw_png("egs_"+std::string(buf)+".png", 4096, 4096);
  }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
  static glm::vec3 pos_old = {0, 0, 0};
  int width, height;
  glfwGetWindowSize(window, &width, &height);
  glm::vec3 pos_new(xpos/width, ypos/-height, 0);
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) && pos_old != pos_new) {
    glm::vec3 d(pos_new.y - pos_old.y, - pos_new.x + pos_old.x, 0);
    double len = glm::length(d);
    if (len > 0) {
      static_cast<GLFWContext*>(glfwGetWindowUserPointer(window))->rotate(glm::normalize(d), len);
    }
  }
  pos_old = pos_new;
}

void scroll_callback(GLFWwindow *window, double x_scroll, double y_scroll) {
  (void) window;
  (void) x_scroll;
  y_scroll = fmax(fmin(y_scroll, 1), -1);
  static_cast<GLFWContext*>(glfwGetWindowUserPointer(window))->zoom(1-0.1*y_scroll);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  (void) window;
  (void) width;
  (void) height;
  glViewport(0, 0, width, height);
  static_cast<GLFWContext*>(glfwGetWindowUserPointer(window))->set_perspective(45, 1.0*width/height);
}

GLFWContext::GLFWContext(Context &ctx) : GLContext(ctx) {
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
  window = glfwCreateWindow(600, 600, "EGS", 0, 0);
  window_count++;

  if (!window) {
    std::cerr << "Error while creating window" << std::endl;
    glfwTerminate();
  }
  glfwMakeContextCurrent(window);

  glip_context_t *glip_ctx = glip_init_gl();
  assert(glip_ctx);
  glip_set_current_context(glip_ctx);

  glfwSetWindowUserPointer(window, this);
  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, cursor_position_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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

egs_glfw_context_ref egs_glfw_context_create(egs_context_ref ctx_ref) {
  Context *ctx = reinterpret_cast<Context *>(ctx_ref);
  GLFWContext *glfw_ctx = new GLFWContext(*ctx);
  return reinterpret_cast<egs_glfw_context_ref>(glfw_ctx);
}

void egs_glfw_context_destroy(egs_glfw_context_ref glfw_ctx) {
  delete reinterpret_cast<GLFWContext *>(glfw_ctx);
}

#endif
