//
//  glfw_context.cpp
//  egs
//
//  Created by Daniel Kaiser on 15/06/16.
//  Copyright © 2016 Daniel Kaiser. All rights reserved.
//

#ifndef NO_GLFW_CONTEXT

#include "glip_glfw_context.hxx"
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <memory>
#include <unistd.h>
#include "glip_glfw_protocol_impl.h"

#define ZMQ_CONN "ipc:///tmp/glip.sock."

std::string path_join(const std::string &a, const std::string &b) {
  std::string res = "";
  auto end_a = a.end();
  while (*(--end_a) == '/')
    ;
  res.append(a.begin(), end_a+1);
  res.push_back('/');
  auto start_b = b.begin();
  while (*start_b == '/') {
    start_b++;
  }
  res.append(start_b, b.end());
  return res;
}

GLIPGLFWContext::GLIPGLFWContext(Context &ctx) : GLContext(ctx) {
  pid_t pid = fork();
  assert(pid != -1);
  if(!pid) { // new glip server process
    pid_t server_pid = getpid();
    std::string connection_str = std::string(ZMQ_CONN)+std::to_string(server_pid);
    std::string glip_path = ".";
    if (getenv("GLIP_SERVER")) {
      glip_path = std::string(getenv("GLIP_SERVER"));
    } else if (getenv("EGS_PATH")) {
      glip_path = path_join(std::string(getenv("EGS_PATH")), "/../bin/");
    }
    glip_path = path_join(glip_path, "/glip_glfw_server ")+connection_str;
    egs_printf(EGS_DEBUG, "executing: \"%s\"\n", glip_path.c_str());
    int res = system(glip_path.c_str());
    exit(res);
  }
  std::string connection_str = std::string(ZMQ_CONN)+std::to_string(pid);
  glip_ctx = glip_init(0, connection_str.c_str());
  assert(glip_ctx);
  glip_set_current_context(glip_ctx);
  glEnable(GL_DEPTH_TEST);
  /*glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);*/
}

bool GLIPGLFWContext::update(std::shared_ptr<DisplayList> display_list) {
  glip_set_current_context(glip_ctx);

  glClearColor(1, 1, 1, 1);
  if (glip_ctx->context_state != GLIP_STATE_OK) {
    return false;
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  assert(glGetError() == 0);

  display_list->apply(*this);
  glip_swap_buffers();

  //return !glfwWindowShouldClose(window);
  return (glip_ctx->context_state == GLIP_STATE_OK);
}

GLIPGLFWContext::~GLIPGLFWContext() {
  if (glip_ctx) {
    glip_destroy(glip_ctx);
  }
}

egs_glip_glfw_context_ref egs_glip_glfw_context_create(egs_context_ref ctx_ref) {
  Context *ctx = reinterpret_cast<Context *>(ctx_ref);
  GLIPGLFWContext *glfw_ctx = new GLIPGLFWContext(*ctx);
  return reinterpret_cast<egs_glip_glfw_context_ref>(glfw_ctx);
}

void egs_glip_glfw_context_destroy(egs_glip_glfw_context_ref glfw_ctx) {
  delete reinterpret_cast<GLIPGLFWContext *>(glfw_ctx);
}

#endif
