//
//  gl_offscreen_context.cpp
//  egs
//
//  Created by Daniel Kaiser on 15/06/16.
//  Copyright © 2016 Daniel Kaiser. All rights reserved.
//

#include "gl_offscreen_context.hxx"
#include <glm/gtx/transform.hpp>
#include "glip.h"

egs_gloffscreen_context_ref egs_gloffscreen_context_create(egs_context_ref ctx_ref) {
  Context *ctx = reinterpret_cast<Context *>(ctx_ref);
  GLOffscreenRenderer *glfw_ctx = new GLOffscreenRenderer(*ctx);
  return reinterpret_cast<egs_gloffscreen_context_ref>(glfw_ctx);
}
void egs_gloffscreen_context_destroy(egs_gloffscreen_context_ref gl_ctx) {
  delete reinterpret_cast<GLOffscreenRenderer *>(gl_ctx);
}

int *egs_gloffscreen_get_data(egs_gloffscreen_context_ref gl_ctx) {
  return reinterpret_cast<GLOffscreenRenderer *>(gl_ctx)->get_pixel_data();
}

GLOffscreenRenderer::GLOffscreenRenderer(Context &ctx) : GLContext(ctx) {
#if __APPLE__
  CGLPixelFormatObj pix; /* pixel format */
  GLint npix; /* number of virtual screens referenced by pix after
               call to CGLChoosePixelFormat*/
  //const CGLPixelFormatAttribute pf_attributes[] = {kCGLPFAColorSize, (CGLPixelFormatAttribute)24, kCGLPFAAlphaSize, (CGLPixelFormatAttribute)8, kCGLPFADepthSize, (CGLPixelFormatAttribute)24, kCGLPFAPBuffer, (CGLPixelFormatAttribute)0, (CGLPixelFormatAttribute)0};
  CGLPixelFormatAttribute pf_attributes[13] = {
    kCGLPFAOpenGLProfile, (CGLPixelFormatAttribute)kCGLOGLPVersion_GL3_Core,
    kCGLPFAColorSize,     (CGLPixelFormatAttribute)24,
    kCGLPFAAlphaSize,     (CGLPixelFormatAttribute)8,
    kCGLPFAAccelerated,
    kCGLPFADoubleBuffer,
    kCGLPFASampleBuffers, (CGLPixelFormatAttribute)1,
    kCGLPFASamples,       (CGLPixelFormatAttribute)4,
    (CGLPixelFormatAttribute)0
  };
  CGLChoosePixelFormat(pf_attributes, &pix, &npix);

  CGLCreateContext(pix, NULL, &cgl_ctx);
  CGLReleasePixelFormat(pix);

  CGLSetCurrentContext(cgl_ctx);
  CGLLockContext(cgl_ctx);
#elif __linux__
  int major, minor;
  int fbcount;
  GLXFBConfig *fbc;
  GLXFBConfig fbconfig;
  static Display *display; /*!< The used X display */
  static Pixmap pixmap; /*!< The XPixmap (GLX < 1.4)*/
  static GLXPbuffer pbuffer; /*!< The GLX Pbuffer (GLX >=1.4) */
  static GLXContext context; /*!< The GLX context */

  display = XOpenDisplay(0);
  if (!display) {
    egs_printf(EGS_ERROR, "Not connected to an X server!");
  }


  glXQueryVersion(display,&major,&minor);
  if (major > 1 || minor >=4) {
    int fb_attribs[] =
    {
      GLX_DRAWABLE_TYPE   , GLX_PBUFFER_BIT,
      GLX_RENDER_TYPE     , GLX_RGBA_BIT,
      None
    };
    int pbuffer_attribs[] =
    {
      GLX_PBUFFER_WIDTH, 1,
      GLX_PBUFFER_HEIGHT, 1,
      None
    };
    egs_printf(EGS_DEBUG, "(Pbuffer)");

    fbc = glXChooseFBConfig(display, DefaultScreen(display), fb_attribs,
                            &fbcount);
    if (fbcount == 0) {
      egs_printf(EGS_ERROR, "failed to find a valid a GLX FBConfig for a RGBA PBuffer");
      XFree(fbc);
      XCloseDisplay(display);
    }
    fbconfig = fbc[0];
    XFree(fbc);
    pbuffer = glXCreatePbuffer(display, fbconfig, pbuffer_attribs);

    const int attrib_list[] = {GLX_CONTEXT_MAJOR_VERSION_ARB, 3, GLX_CONTEXT_MINOR_VERSION_ARB, 3, GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB, 0, 0};
    context = ((GLXContext (*)(Display *, GLXFBConfig, GLXContext, Bool, const int *))glXGetProcAddress((const GLubyte *)"glXCreateContextAttribsARB"))(display, fbconfig, None, True, attrib_list);
    glXMakeContextCurrent(display,pbuffer,pbuffer,context);
  }

#endif
  glip_context_t *glip_ctx = glip_init_gl();
  assert(glip_ctx);
  glip_set_current_context(glip_ctx);

  width = 500;
  height = 500;
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

  glViewport(0, 0, width, height);

  pixels = (int *)malloc(width*height*sizeof(int));
}

int GLOffscreenRenderer::get_width() {
  return 500;
}

int GLOffscreenRenderer::get_height() {
  return 500;
}

bool GLOffscreenRenderer::update(std::shared_ptr<DisplayList> display_list) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  display_list->apply(*this);
  return true;
}

int *GLOffscreenRenderer::get_pixel_data() {
  glReadBuffer(GL_COLOR_ATTACHMENT0);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, fb);
  assert(!glGetError());
  glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  return pixels;
}
