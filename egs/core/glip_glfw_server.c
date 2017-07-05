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
#include <stdlib.h>
#include <stdio.h>
#include <zmq.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <GLFW/glfw3.h>
#include <glip.h>
#include <signal.h>
#include "glip_glfw_protocol_impl.h"
#include "glip_event_list.h"

extern void (*const glip_implementations[])(const char *, char**, size_t *);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
  size_t data_size = 5*sizeof(int);
  unsigned char *data = (unsigned char *)malloc(data_size);
  ((int *)data)[0] = GLIP_EVENT_KEY;
  ((int *)data)[1] = key;
  ((int *)data)[2] = scancode;
  ((int *)data)[3] = action;
  ((int *)data)[4] = mods;
  event_list_add_event((event_list_t *)glfwGetWindowUserPointer(window), data_size, data);
  free(data);
}

void cursor_callback(GLFWwindow *window, double xpos, double ypos) {
  size_t data_size = sizeof(int)+2*sizeof(double);
  unsigned char *data = (unsigned char *)malloc(data_size);
  ((int *)data)[0] = GLIP_EVENT_MOUSE;
  ((double *)(data+sizeof(int)))[0] = xpos;
  ((double *)(data+sizeof(int)))[1] = ypos;
  event_list_add_event((event_list_t *)glfwGetWindowUserPointer(window), data_size, data);
  free(data);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  size_t data_size = sizeof(int)+2*sizeof(double);
  unsigned char *data = (unsigned char *)malloc(data_size);
  ((int *)data)[0] = GLIP_EVENT_SCROLL;
  ((double *)(data+sizeof(int)))[0] = xoffset;
  ((double *)(data+sizeof(int)))[1] = yoffset;
  event_list_add_event((event_list_t *)glfwGetWindowUserPointer(window), data_size, data);
  free(data);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  size_t data_size = 3*sizeof(int);
  unsigned char *data = (unsigned char *)malloc(data_size);
  ((int *)data)[0] = GLIP_EVENT_WINDOW_SIZE;
  ((int *)data)[1] = width;
  ((int *)data)[2] = height;
  event_list_add_event((event_list_t *)glfwGetWindowUserPointer(window), data_size, data);
  free(data);
}

int main(int argc, char *argv[]) {
  int rc;
  int glip_glfw_terminated = 0;
  int parent_process = getppid();
  if (argc < 2) {
    fprintf(stderr, "usage: %s <zmq-socket>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  int width = -1, height = -1, pos_x = -1, pos_y = -1;
  if (argc > 2 && (argc % 2) == 0) {
    int arg;
    for (arg = 2; arg < argc; arg+=2) {
      if (strcmp(argv[arg], "width") == 0) {
        width = atoi(argv[arg+1]);
      } else if (strcmp(argv[arg], "height") == 0) {
        height = atoi(argv[arg+1]);
      } else if (strcmp(argv[arg], "posx") == 0) {
        pos_x = atoi(argv[arg+1]);
      } else if (strcmp(argv[arg], "posy") == 0) {
        pos_y = atoi(argv[arg+1]);
      }
    }
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
  if (width < 0) {
    width = 600;
  }
  if (height < 0) {
    height = 600;
  }
  GLFWwindow *window = glfwCreateWindow(width, height, "EGS", 0, 0);

  if (!window) {
    fprintf(stderr, "Error while creating window\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  if (pos_x >= 0 && pos_y >= 0) {
    glfwSetWindowPos(window, pos_x, pos_y);
  }

  event_list_t *event_list = event_list_create();
  glfwSetWindowUserPointer(window, event_list);
  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, cursor_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  printf("glip glfw server connecting on: %s\n", argv[1]);
  glip_context_t *glip_ctx = glip_init(1, argv[1]);
  glip_set_current_context(glip_ctx);

  while(!glip_glfw_terminated && parent_process == getppid()) {
    zmq_msg_t msg;
    zmq_msg_init(&msg);
    glfwPollEvents();

    rc = zmq_recvmsg(glip_ctx->zmq_sock, &msg, ZMQ_NOBLOCK);
    if (rc >= 0) {
      uint32_t *func_data = (uint32_t *)zmq_msg_data(&msg);
      glip_debug("server got message of length %d from client %s. function is %d\n", rc, argv[1], *func_data);
      const char *msg_data = (const char*)(func_data+1);
      size_t res_len = (size_t)-1;
      char *res = NULL;
      if (*func_data < GLIP_GLFW_FUNCTION_OFFSET) {
        glip_implementations[*func_data](msg_data, &res, &res_len);
      } else {
        switch (*func_data) {
          case GLIP_GLFW_SWAP_BUFFER:
            glip_debug("swap buffer\n");
            glfwSwapBuffers(window);
            break;
          case GLIP_GLFW_WINDOW_SHOULD_CLOSE:
            glip_debug("check window_should_close\n");
            int glip_glfw_window_should_close = glfwWindowShouldClose(window);
            res_len = sizeof(int);
            res = (char *)malloc(res_len);
            assert(res);
            ((int *)res)[0] = glip_glfw_window_should_close;
            break;
          case GLIP_GLFW_TERMINATE_WINDOW:
            glip_debug("glfw terminate\n");
            glip_glfw_terminated = 1;
            break;
          case GLIP_GLFW_GET_WINDOW_SIZE:
            glip_debug("get window size\n");
            glfwGetWindowSize(window, &width, &height);
            res_len = 2*sizeof(int);
            res = (char *)malloc(res_len);
            assert(res);
            ((int *)res)[0] = width;
            ((int *)res)[1] = height;
            break;
          case GLIP_GLFW_GET_EVENTS:
            glip_debug("get events\n");
            res_len = event_list->current_length;
            res = (char *)malloc(res_len);
            assert(res);
            memcpy(res, event_list->data, event_list->current_length);
            event_list_clear(event_list);
            break;
          case GLIP_GLFW_GET_MOUSE_BUTTON:
            glip_debug("get window size\n");
            res_len = sizeof(int);
            res = (char *)malloc(res_len);
            assert(res);
            ((int *)res)[0] = glfwGetMouseButton(window, *((int *)msg_data));
            break;
          default:
            fprintf(stderr, "Unknown GLFW function\n");
        }
      }
      if (res_len != (size_t)-1 && res) {
        zmq_msg_t return_msg;
        zmq_msg_init_data (&return_msg, res, res_len, glip_zmq_msg_free, NULL);
        zmq_sendmsg(glip_ctx->zmq_sock, &return_msg, 0);
      }
    } else if (errno != EAGAIN) {
      fprintf(stderr, "zmq error\n");
    } else {
      usleep(1000);
    }
    zmq_msg_close (&msg);
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  event_list_destroy(event_list);
  glip_destroy(glip_ctx);
}
