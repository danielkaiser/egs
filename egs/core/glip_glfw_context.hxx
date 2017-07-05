//
//  glfw_context.hpp
//  egs
//
//  Created by Daniel Kaiser on 15/06/16.
//  Copyright © 2016 Daniel Kaiser. All rights reserved.
//

#ifndef GLIP_GLFW_CONTEXT_H
#define GLIP_GLFW_CONTEXT_H

#include <stdio.h>
#include "glip.h"
#include <GLFW/glfw3.h>
#include "gl_context.hxx"
#include "gl_interactivity_mixin.hxx"
#include <thread>
#include <mutex>

class GLIPGLFWContext : public GLContext, public GLInteractivityMixin {
public:
  GLIPGLFWContext(Context &ctx) : GLIPGLFWContext(ctx, -1, -1, -1, -1) {}
  GLIPGLFWContext(Context &ctx, int width, int height) : GLIPGLFWContext(ctx, width, height, -1, -1) {}
  GLIPGLFWContext(Context &ctx, int width, int height, int pos_x, int pos_y);
  GLIPGLFWContext(const GLIPGLFWContext &) = delete;
  ~GLIPGLFWContext();

  virtual bool update(std::shared_ptr<DisplayList>);

  int get_width();
  int get_height();
private:
  bool draw_update(std::shared_ptr<DisplayList>);
  static void handle_events(GLIPGLFWContext *ctx);
  bool run_event_handling_thread = true;

  glip_context_t *glip_ctx = nullptr;
  std::thread event_handling_thread;
  static std::mutex glip_mutex;
  std::shared_ptr<DisplayList> last_display_list;
  bool needs_update = false;
};

#endif /* glfw_context_hpp */
