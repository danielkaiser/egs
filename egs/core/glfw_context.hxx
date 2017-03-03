//
//  glfw_context.hpp
//  egs
//
//  Created by Daniel Kaiser on 15/06/16.
//  Copyright © 2016 Daniel Kaiser. All rights reserved.
//

#ifndef GLFW_CONTEXT_H
#define GLFW_CONTEXT_H

#include <stdio.h>
#include "glip.h"
#include <GLFW/glfw3.h>
#include "gl_interactivity_mixin.hxx"
#include "gl_context.hxx"
#include <memory>

class GLFWContext : public GLContext, public GLInteractivityMixin {
public:
  GLFWContext(Context &ctx);
  GLFWContext(Context &ctx, int width, int height);
  GLFWContext(Context &ctx, int width, int height, int pos_x, int pos_y);
  ~GLFWContext();

  virtual bool update(std::shared_ptr<DisplayList>);

  int get_width();
  int get_height();
private:
  static unsigned int window_count;
  GLFWwindow *window = nullptr;
};

#endif /* glfw_context_hpp */
