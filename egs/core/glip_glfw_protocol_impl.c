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
#include <assert.h>

extern glip_context_t* _glip_current_context;

void glip_swap_buffers() {
  if (_glip_current_context->is_local) {
    glip_debug("glip_swap_buffers is local\n");
  } else {
    size_t data_size = sizeof(uint32_t);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = GLIP_GLFW_SWAP_BUFFER;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

int glip_window_should_close() {
  if (_glip_current_context->is_local) {
    glip_debug("glip_window_should_close is local\n");
    return 0;
  } else {
    size_t data_size = sizeof(uint32_t);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = GLIP_GLFW_WINDOW_SHOULD_CLOSE;
    glip_send(_glip_current_context, data_size, data);
    free(data);
    glip_recv(_glip_current_context, &data_size, &data);
    int result = ((int *)data)[0];
    free(data);
    return result;
  }
}

void glip_glfw_terminate() {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glfw_terminate is local\n");
  } else {
    size_t data_size = sizeof(uint32_t);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = GLIP_GLFW_TERMINATE_WINDOW;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

void glip_glfw_get_window_size(int *width, int *height) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glfw_get_window_size is local\n");
  } else {
    size_t data_size = sizeof(uint32_t);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = GLIP_GLFW_GET_WINDOW_SIZE;
    glip_send(_glip_current_context, data_size, data);
    free(data);
    glip_recv(_glip_current_context, &data_size, &data);
    *width = ((int *)data)[0];
    *height = ((int *)data)[1];
    free(data);
  }
}

event_list_t *glip_glfw_get_events() {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glfw_get_events is local\n");
    return NULL;
  } else {
    size_t data_size = sizeof(uint32_t);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = GLIP_GLFW_GET_EVENTS;
    glip_send(_glip_current_context, data_size, data);
    free(data);
    glip_recv(_glip_current_context, &data_size, &data);
    assert(data != NULL && data_size > 0);
    event_list_t *list = event_list_create_from_char_ptr(data_size, ((unsigned char *)data));
    free(data);
    return list;
  }
}

int glip_glfw_get_mouse_button(int button) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glfw_get_mouse_button is local\n");
    return 0;
  } else {
    size_t data_size = 2*sizeof(uint32_t);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = GLIP_GLFW_GET_MOUSE_BUTTON;
    ((int*)data)[1] = button;
    glip_send(_glip_current_context, data_size, data);
    free(data);
    glip_recv(_glip_current_context, &data_size, &data);
    int result = ((int *)data)[0];
    free(data);
    return result;
  }
}
