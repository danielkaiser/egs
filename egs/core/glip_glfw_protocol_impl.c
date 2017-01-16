//
//  glip_glfw_protocol_impl.c
//  egs
//
//  Created by Daniel Kaiser on 09/09/16.
//  Copyright © 2016 Daniel Kaiser. All rights reserved.
//

#include <stdlib.h>

#include "glip.h"
#include "glip_glfw_protocol_impl.h"

extern glip_context_t* _glip_current_context;

void glip_swap_buffers() {
  if (_glip_current_context->is_local) {
    glip_debug("glip_swap_buffers is local\n");
  } else {
    size_t data_size = sizeof(uint32_t);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = GLFW_SWAP_BUFFER;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}
