//
//  glip_glfw_protocol_impl.h
//  egs
//
//  Created by Daniel Kaiser on 09/09/16.
//  Copyright © 2016 Daniel Kaiser. All rights reserved.
//

#ifndef glip_glfw_protocol_impl_h
#define glip_glfw_protocol_impl_h

#include <stdio.h>
#include "glip_event_list.h"

#define GLIP_GLFW_FUNCTION_OFFSET 10000
enum {
  GLIP_GLFW_SWAP_BUFFER         = GLIP_GLFW_FUNCTION_OFFSET+1,
  GLIP_GLFW_WINDOW_SHOULD_CLOSE = GLIP_GLFW_FUNCTION_OFFSET+2,
  GLIP_GLFW_TERMINATE_WINDOW    = GLIP_GLFW_FUNCTION_OFFSET+3,
  GLIP_GLFW_GET_WINDOW_SIZE     = GLIP_GLFW_FUNCTION_OFFSET+4,
  GLIP_GLFW_GET_EVENTS          = GLIP_GLFW_FUNCTION_OFFSET+5,
  GLIP_GLFW_GET_MOUSE_BUTTON    = GLIP_GLFW_FUNCTION_OFFSET+6,
};

#ifdef __cplusplus
extern "C" {
#endif

  void glip_swap_buffers();
  int glip_window_should_close();
  void glip_glfw_terminate();
  void glip_glfw_get_window_size(int *width, int *height);
  event_list_t *glip_glfw_get_events();
  int glip_glfw_get_mouse_button(int button);

#ifdef __cplusplus
}
#endif

  
#endif /* glip_glfw_protocol_impl_h */
