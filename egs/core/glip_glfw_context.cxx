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
#include <thread>
#include <functional>
#include <chrono>
#include <errno.h>
#include "glip_event_list.h"

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

std::mutex GLIPGLFWContext::glip_mutex;

GLIPGLFWContext::GLIPGLFWContext(Context &ctx, int width, int height, int pos_x, int pos_y) : GLContext(ctx), GLInteractivityMixin(static_cast<GLContext &>(*this)) {
  pid_t pid = fork();
  assert(pid != -1);
  if(!pid) { // new glip server process
    pid_t server_pid = getpid();
    std::string connection_str = std::string(ZMQ_CONN)+std::to_string(server_pid);
    std::string glip_path;
    if (getenv("GLIP_SERVER")) {
      glip_path = std::string(getenv("GLIP_SERVER"));
    } else if (getenv("EGS_PATH")) {
      glip_path = path_join(std::string(getenv("EGS_PATH")), "/../bin/");
    }

    int res;
    std::string cmd = path_join(glip_path, "/glip_glfw_server");
    if (pos_x >= 0 && pos_y >= 0 && width >= 0 && height >= 0) {
      const char * width_str = std::to_string(width).c_str();
      const char * height_str = std::to_string(height).c_str();
      const char * posx_str = std::to_string(pos_x).c_str();
      const char * posy_str = std::to_string(pos_y).c_str();
      res = execl(cmd.c_str(), "glip_glfw_server", connection_str.c_str(), "width", width_str, "height", height_str, "posx", posx_str, "posy", posy_str, (char *)0);
    } else if (width >= 0 && height >= 0) {
      const char * width_str = std::to_string(width).c_str();
      const char * height_str = std::to_string(height).c_str();
      res = execl(cmd.c_str(), "glip_glfw_server", connection_str.c_str(), "width", width_str, "height", height_str, (char *)0);
    } else {
      res = execl(cmd.c_str(), "glip_glfw_server", connection_str.c_str(), (char *)0);
    }
    perror(NULL);
    exit(res);
  }
  std::string connection_str = std::string(ZMQ_CONN)+std::to_string(pid);
  {
    std::lock_guard<std::mutex> lock_guard(GLIPGLFWContext::glip_mutex);
    glip_ctx = glip_init(0, connection_str.c_str());
    assert(glip_ctx);
    glip_set_current_context(glip_ctx);
    glEnable(GL_DEPTH_TEST);
  }
  event_handling_thread = std::thread(handle_events, this);
}

bool GLIPGLFWContext::draw_update(std::shared_ptr<DisplayList> display_list) {
  glip_set_current_context(glip_ctx);
  glClearColor(1, 1, 1, 1);
  if (glip_ctx->context_state != GLIP_STATE_OK) {
    return false;
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  assert(glGetError() == 0);

  display_list->apply(*this);

  glip_swap_buffers();

  last_display_list = display_list;
  needs_update = false;
  if (glip_window_should_close()) {
    return false;
  }

  return (glip_ctx->context_state == GLIP_STATE_OK);
}

bool GLIPGLFWContext::update(std::shared_ptr<DisplayList> display_list) {
  std::lock_guard<std::mutex> lock_guard(GLIPGLFWContext::glip_mutex);
  bool result = draw_update(display_list);
  return result;
}

GLIPGLFWContext::~GLIPGLFWContext() {
  std::lock_guard<std::mutex> lock_guard(GLIPGLFWContext::glip_mutex);
  glip_set_current_context(glip_ctx);
  run_event_handling_thread = false;
  event_handling_thread.join();
  call_delete_handler();
  glip_glfw_terminate();
  if (glip_ctx) {
    glip_destroy(glip_ctx);
  }

}

int GLIPGLFWContext::get_width() {
  int width, height;
  glip_glfw_get_window_size(&width, &height);
  return width;
}

int GLIPGLFWContext::get_height() {
  int width, height;
  glip_glfw_get_window_size(&width, &height);
  return width;
}

void GLIPGLFWContext::handle_events(GLIPGLFWContext *ctx) {
  while (ctx->run_event_handling_thread) {
    {
      std::lock_guard<std::mutex> lock_guard(GLIPGLFWContext::glip_mutex);
      glip_set_current_context(ctx->glip_ctx);
      event_list_t *event_list = glip_glfw_get_events();
      assert(event_list);
      for (int i = 0; i < event_list_get_event_count(event_list); i++) {
        size_t data_length;
        unsigned char *event = event_list_get_event(event_list, i, &data_length);
        if (event == NULL) {
          continue;
        }
        assert(data_length != 0);
        int event_type = *((int *) event);
        unsigned char *data = event + sizeof(int);
        switch (event_type) {
          case GLIP_EVENT_KEY:
            assert(data_length == 5 * sizeof(int));
            ctx->key_callback(*ctx, ((int *) data)[0], ((int *) data)[2]);
            break;
          case GLIP_EVENT_MOUSE:
            assert(data_length == sizeof(int) + 2 * sizeof(double));
            ctx->mouse_callback(*ctx, ((double *) data)[0], ((double *) data)[1],
                                glip_glfw_get_mouse_button(EGS_MOUSE_BUTTON_LEFT));
            break;
          case GLIP_EVENT_SCROLL:
            assert(data_length == sizeof(int) + 2 * sizeof(double));
            ctx->scroll_callback(*ctx, ((double *) data)[0], ((double *) data)[1]);
            break;
          case GLIP_EVENT_WINDOW_SIZE:
            assert(data_length == 3 * sizeof(int));
            ctx->resize_callback(*ctx, ((int *) data)[0], ((int *) data)[1]);
            break;
          default:
            egs_printf(EGS_WARNING, "Unknown glip event %d\n", event_type);
            break;
        }
        ctx->needs_update = true;
      }
    }
    if (ctx->needs_update && ctx->last_display_list) {
      /* TODO */
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

  }
}

egs_glip_glfw_context_ref egs_glip_glfw_context_create(egs_context_ref ctx_ref, int width, int height, int pos_x, int pos_y) {
  Context *ctx = reinterpret_cast<Context *>(ctx_ref);
  GLIPGLFWContext *glfw_ctx = new GLIPGLFWContext(*ctx, width, height, pos_x, pos_y);
  return reinterpret_cast<egs_glip_glfw_context_ref>(glfw_ctx);
}

void egs_glip_glfw_context_destroy(egs_glip_glfw_context_ref glfw_ctx) {
  delete reinterpret_cast<GLIPGLFWContext *>(glfw_ctx);
}

#endif
