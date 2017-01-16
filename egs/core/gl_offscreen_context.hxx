//
//  gl_offscreen_context.hpp
//  egs
//
//  Created by Daniel Kaiser on 15/06/16.
//  Copyright © 2016 Daniel Kaiser. All rights reserved.
//

#ifndef GL_OFFSCREEN_CONTEXT
#define GL_OFFSCREEN_CONTEXT

#include <stdio.h>
#include "gl_context.hxx"

#if __APPLE__
#include <OpenGL/OpenGL.h>
/* OpenGL.h in Mac OS X 10.7 doesn't include gl.h anymore */
#include <OpenGL/gl.h>
#elif __linux__
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glx.h>
#endif

class GLOffscreenRenderer : public GLContext {
public:
  GLOffscreenRenderer(Context &ctx);
  
  virtual bool update(std::shared_ptr<DisplayList>);
  int *get_pixel_data();
  
private:
#if __APPLE__
  CGLContextObj cgl_ctx;
#elif __linux__
  
#endif
  
  GLuint fb, rb, db;
  int width, height;
  int *pixels;
};

#endif /* gl_offscreen_context_hpp */
