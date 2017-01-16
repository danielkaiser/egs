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

class GLFWContext : public GLContext {
public:
  GLFWContext(Context &ctx);
  ~GLFWContext();

  virtual bool update(std::shared_ptr<DisplayList>);
private:
  GLFWwindow *window = nullptr;
};

#endif /* glfw_context_hpp */
