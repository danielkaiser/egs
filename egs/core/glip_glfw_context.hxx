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
#include "gl_context.hxx"

class GLIPGLFWContext : public GLContext {
public:
  GLIPGLFWContext(Context &ctx);
  ~GLIPGLFWContext();

  virtual bool update(std::shared_ptr<DisplayList>);

  int get_width();
  int get_height();
private:
  glip_context_t *glip_ctx = nullptr;
};

#endif /* glfw_context_hpp */
