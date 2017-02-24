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

#define GLFW_FUNCTION_OFFSET 10000
#define GLFW_SWAP_BUFFER (GLFW_FUNCTION_OFFSET+1)
#define GLFW_WINDOW_SHOULD_CLOSE (GLFW_FUNCTION_OFFSET+2)
#define GLFW_TERMINATE_WINDOW (GLFW_FUNCTION_OFFSET+3)

#ifdef __cplusplus
extern "C" {
#endif

void glip_swap_buffers();
int glip_window_should_close();
void glip_glfw_terminate();

#ifdef __cplusplus
}
#endif

  
#endif /* glip_glfw_protocol_impl_h */
