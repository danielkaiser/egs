//
//  glip_zmq.c
//  egs
//
//  Created by Daniel Kaiser on 01/07/16.
//  Copyright © 2016 Daniel Kaiser. All rights reserved.
//

#include "glip_glfw_server.h"
#include "glip.h"
#include "glad.h"
#include "glip_impl.h"
#include <stdlib.h>
#include <stdio.h>
#include <zmq.h>
#include <assert.h>
#include <GLFW/glfw3.h>
#include "glip_glfw_protocol_impl.h"

void glip_zmq_msg_free(void *data, void *hint) {
  (void) hint;
  free (data);
}

int main(int argc, char *argv[]) {
  int rc;
  if (argc < 2) {
    fprintf(stderr, "usage: %s <zmq-socket>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  if (!glfwInit()) {
    fprintf(stderr, "Error while glfwinit\n");
    exit(EXIT_FAILURE);
  }
  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_SAMPLES, 4);
  GLFWwindow *window = glfwCreateWindow(600, 600, "EGS", 0, 0);

  if (!window) {
    fprintf(stderr, "Error while creating window\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  glip_context_t *glip_ctx = glip_init(1, argv[1]);
  glip_set_current_context(glip_ctx);

  while(!glfwWindowShouldClose(window)) {
    zmq_msg_t msg;
    zmq_msg_init(&msg);
    glfwPollEvents();

    rc = zmq_recvmsg(glip_ctx->zmq_sock, &msg, ZMQ_NOBLOCK);
    if (rc >= 0) {
      uint32_t *func_data = (uint32_t *)zmq_msg_data(&msg);
      glip_debug("server got message of length %d from client. function is %d\n", rc , *func_data);
      const char *msg_data = (const char*)(func_data+1);
      size_t res_len = (size_t)-1;
      char *res = NULL;
      if (*func_data < GLFW_FUNCTION_OFFSET) {
        glip_implementations[*func_data](msg_data, &res, &res_len);
      } else {
        switch (*func_data) {
          case GLFW_SWAP_BUFFER:
            glip_debug("swap buffer\n");
            glfwSwapBuffers(window);
            break;
          default:
            fprintf(stderr, "Unknown GLFW function\n");
        }
      }
      if (res_len != (size_t)-1 && res) {
        zmq_msg_t return_msg;
        zmq_msg_init_data (&return_msg, res, res_len, glip_zmq_msg_free, (void *)NULL);
        zmq_sendmsg(glip_ctx->zmq_sock, &return_msg, 0);
      }
    } else if (errno != EAGAIN) {
      fprintf(stderr, "zmq error\n");
    }
    zmq_msg_close (&msg);
  }

  glip_destroy(glip_ctx);
}
