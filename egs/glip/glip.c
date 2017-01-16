/*
//  glip.c
//  egs
//
//  Created by Daniel Kaiser on 01/07/16.
//  Copyright © 2016 Daniel Kaiser. All rights reserved.
*/

#include "glip.h"
#include "glad.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <zmq.h>
#include <stdarg.h>
#include <execinfo.h>
#include <unistd.h>

/*void *loader_wrapper(const char *name) {
 glip_debug("loader wrapper called for %s\n", name);
 return get_proc(name);
 }
 
 int gladLoadGL(void) {
 int status = 0;
 
 if(open_gl()) {
 status = gladLoadGLLoader(loader_wrapper);
 close_gl();
 }
 
 return status;
 }*/

glip_context_t *glip_init(int is_local, const char* connection_str) {
  int rc;
  glip_context_t *glip_ctx = (glip_context_t *)malloc(sizeof(glip_context_t));
  glip_ctx->zmq_context = zmq_init(1);
  assert(glip_ctx->zmq_context);
  glip_ctx->zmq_sock = zmq_socket(glip_ctx->zmq_context, ZMQ_PAIR);
  assert(glip_ctx->zmq_sock);
  if (is_local) { // local execution of gl functions -> server process
    assert(gladLoadGL());
    glip_debug("server binding %s\n", connection_str);
    rc = zmq_bind(glip_ctx->zmq_sock, connection_str);
  } else { // remote execution of gl functions -> client process
    glip_debug("client connecting to %s\n", connection_str);
    rc = zmq_connect(glip_ctx->zmq_sock, connection_str);
  }
  assert(rc == 0);
  glip_ctx->is_local = is_local;
  glip_ctx->context_state = GLIP_STATE_OK;
  return glip_ctx;
}

glip_context_t *glip_init_gl() {
  glip_context_t *glip_ctx = (glip_context_t *)malloc(sizeof(glip_context_t));
  glip_ctx->zmq_context = NULL;
  glip_ctx->zmq_sock = NULL;
  assert(gladLoadGL());
  glip_ctx->is_local = 1;
  glip_ctx->context_state = GLIP_STATE_OK;
  return glip_ctx;
}

void glip_destroy(glip_context_t *glip_ctx) {
  zmq_close(glip_ctx->zmq_sock);
  zmq_term(glip_ctx->zmq_context);
  free(glip_ctx);
}

void glip_zmq_msg_free(void *data, void *hint) {
  (void) hint;
  free (data);
}

void glip_zmq_msg_no_free(void *data, void *hint) {
  (void) hint;
  (void) data;
}

int glip_send(glip_context_t *glip_ctx, const size_t data_len, const char *data) {
  long rc;
  if (glip_ctx->context_state != GLIP_STATE_OK) {
    return 0;
  }
  zmq_msg_t msg;
  zmq_msg_init(&msg);
  zmq_msg_init_size(&msg, data_len);
  //zmq_msg_init_data(&msg, (void *)data, data_len, glip_zmq_msg_no_free, (void *)NULL);
  memcpy(zmq_msg_data(&msg), data, data_len);
  rc = zmq_sendmsg(glip_ctx->zmq_sock, &msg, 0);
  glip_debug("sent message of length %d\n", rc);
  zmq_msg_close(&msg);
  if(rc != (long)data_len) {
    fprintf(stderr, "GLIP: warning: zmq message not completely send (%ld of %ld bytes)\n", rc, data_len);
    glip_ctx->context_state |= GLIP_STATE_SEND_ERROR;
    return 0;
  }
  return 1;
}

int glip_recv(glip_context_t *glip_ctx,size_t* data_len, char **data) {
  int rc;
  if (glip_ctx->context_state != GLIP_STATE_OK) {
    return 0;
  }
  zmq_msg_t msg;
  zmq_msg_init(&msg);
  rc = zmq_msg_init(&msg);
  assert(rc == 0);
  
  rc = zmq_recvmsg(glip_ctx->zmq_sock, &msg, 0);
  if (rc <= 0) {
    fprintf(stderr, "GLIP: warning: zmq message not completely received\n");
    *data_len = 0;
    *data = NULL;
    zmq_msg_close(&msg);
    glip_ctx->context_state |= GLIP_STATE_RECV_ERROR;
    return 0;
  }
  *data_len = zmq_msg_size(&msg);
  *data = (char *)malloc(*data_len);
  memcpy(*data, zmq_msg_data(&msg), *data_len);
  zmq_msg_close(&msg);
  return 1;
}

glip_context_t* _glip_current_context;

void glip_set_current_context(glip_context_t *glip_ctx) {
  _glip_current_context = glip_ctx;
}

glip_context_t *glip_get_current_context() {
  return _glip_current_context;
}

//#define GLIPDEBUG
void glip_debug(const char *format_string, ...) {
#if defined GLIPDEBUG
  va_list args;
  va_start(args, format_string);
  va_end(args);
  printf("[GLIP] ");
  vprintf(format_string, args);
#else
  (void) format_string;
#endif
}

GLuint glip_glCreateShader(GLenum type) {
  if (_glip_current_context->is_local) {
    glip_debug("glCreateShader is local\n");
    return glad_glCreateShader(type);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLenum);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glCreateShader;
    ((GLenum*)(data+4))[0] = type;
    glip_send(_glip_current_context, data_size, data);
    free(data);
    glip_recv(_glip_current_context, &data_size, &data);
    return ((GLuint *)data)[0];
  }
}

void glip_glShaderSource(GLuint shader, GLsizei count, const GLchar* const *string, const GLint *length) {
  if (_glip_current_context->is_local) {
    glip_debug("glShaderSource is local\n");
    return glad_glShaderSource(shader, count, (const GLchar**)string, length);
  } else {
    int i;
    GLint *str_lengths = (GLint *)malloc(count * sizeof(GLint));
    size_t sum = 0;
    for (i=0; i<count; i++) {
      if (!length) {
        str_lengths[i] = (GLint)strlen(string[i]);
      } else {
        str_lengths[i] = length[i];
      }
      sum += str_lengths[i];
    }
    size_t data_size = sizeof(uint32_t) + sizeof(GLuint) + sizeof(GLsizei) + sum + sizeof(GLint) * count;
    char *data = (char *)malloc(data_size);
    *((uint32_t*)data) = _glip_glShaderSource;
    size_t offset = sizeof(uint32_t);
    *((GLuint*)(data+offset)) = shader;
    offset += sizeof(GLuint);
    *((GLsizei*)(data+offset)) = count;
    offset += sizeof(GLsizei);
    for (i=0; i<count; i++) {
      *((GLint*)(data+offset)) = str_lengths[i];
      offset += sizeof(GLint);
    }
    for (i=0; i<count; i++) {
      memcpy((char*)(data+offset), string[i], str_lengths[i]);
      offset += str_lengths[i];
    }
    glip_send(_glip_current_context, data_size, data);
    free(data);
    free(str_lengths);
  }
}

void glip_glCompileShader(GLuint shader) {
  if (_glip_current_context->is_local) {
    glip_debug("glCompileShader is local\n");
    return glad_glCompileShader(shader);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLuint);
    char *data = (char *)malloc(data_size);
    *((uint32_t*)data) = _glip_glCompileShader;
    *((GLuint*)(data+sizeof(uint32_t))) = shader;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glGetShaderiv(GLuint shader, GLenum pname, GLint *params) {
  if (_glip_current_context->is_local) {
    glip_debug("glGetShaderiv is local\n");
    return glad_glGetShaderiv(shader, pname, params);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLuint) + sizeof(GLenum);
    char *data = (char *)malloc(data_size);
    *((uint32_t*)data) = _glip_glGetShaderiv;
    *((GLuint*)(data+sizeof(uint32_t))) = shader;
    *((GLenum*)(data+sizeof(uint32_t)+sizeof(GLuint))) = pname;
    glip_send(_glip_current_context, data_size, data);
    free(data);
    glip_recv(_glip_current_context, &data_size, &data);
    glip_debug("glGetShaderiv response is of length %lu, value=%d\n", data_size, *((GLint*)data));
    assert(data_size == sizeof(GLint));
    *params = ((GLint*)data)[0];
  }
}

GLAPI void glip_glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glGetShaderInfoLog is local\n");
    return glad_glGetShaderInfoLog(shader, maxLength, length, infoLog);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLuint) + sizeof(GLsizei);
    char *data = (char *)malloc(data_size);
    *((uint32_t*)data) = _glip_glGetShaderInfoLog;
    *((GLuint*)(data+sizeof(uint32_t))) = shader;
    *((GLsizei*)(data+sizeof(uint32_t)+sizeof(GLuint))) = maxLength;
    glip_send(_glip_current_context, data_size, data);
    free(data);
    glip_recv(_glip_current_context, &data_size, &data);
    *length = ((GLsizei*)data)[0];
    memcpy(infoLog, data+sizeof(GLsizei), *length);
    free(data);
  }
}

GLAPI void glip_glDeleteShader(GLuint shader) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glDeleteShader is local\n");
    return glad_glDeleteShader(shader);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLuint);
    char *data = (char *)malloc(data_size);
    *((uint32_t*)data) = _glip_glDeleteShader;
    *((GLuint*)(data+sizeof(uint32_t))) = shader;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI GLuint glip_glCreateProgram(void) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glCreateProgram is local\n");
    return glad_glCreateProgram();
  } else {
    size_t data_size = sizeof(uint32_t);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glCreateProgram;
    glip_send(_glip_current_context, data_size, data);
    free(data);
    glip_recv(_glip_current_context, &data_size, &data);
    return ((GLuint *)data)[0];
  }
}

GLAPI void glip_glAttachShader(GLuint program,  GLuint shader) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glAttachShader is local\n");
    return glad_glAttachShader(program, shader);
  } else {
    size_t data_size = sizeof(uint32_t) + 2*sizeof(GLuint);
    char *data = (char *)malloc(data_size);
    *((uint32_t*)data) = _glip_glAttachShader;
    *((GLuint*)(data+sizeof(uint32_t))) = program;
    *((GLuint*)(data+sizeof(uint32_t)+sizeof(GLuint))) = shader;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glBindFragDataLocation(GLuint program, GLuint colorNumber, const char * name) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glBindFragDataLocation is local\n");
    return glad_glBindFragDataLocation(program, colorNumber, name);
  } else {
    size_t data_size = sizeof(uint32_t) + 2*sizeof(GLuint) + strlen(name)+1;
    char *data = (char *)malloc(data_size);
    *((uint32_t*)data) = _glip_glBindFragDataLocation;
    *((GLuint*)(data+sizeof(uint32_t))) = program;
    *((GLuint*)(data+sizeof(uint32_t)+sizeof(GLuint))) = colorNumber;
    memcpy(((char*)(data+sizeof(uint32_t)+2*sizeof(GLuint))), name, strlen(name)+1);
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glLinkProgram(GLuint program) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glLinkProgram is local\n");
    return glad_glLinkProgram(program);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLuint);
    char *data = (char *)malloc(data_size);
    *((uint32_t*)data) = _glip_glLinkProgram;
    *((GLuint*)(data+sizeof(uint32_t))) = program;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glGetProgramiv(GLuint program, GLenum pname, GLint *params) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glGetProgramiv is local\n");
    return glad_glGetProgramiv(program, pname, params);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLuint) + sizeof(GLenum);
    char *data = (char *)malloc(data_size);
    *((uint32_t*)data) = _glip_glGetProgramiv;
    *((GLuint*)(data+sizeof(uint32_t))) = program;
    *((GLenum*)(data+sizeof(uint32_t)+sizeof(GLuint))) = pname;
    glip_send(_glip_current_context, data_size, data);
    free(data);
    glip_recv(_glip_current_context, &data_size, &data);
    glip_debug("glip_glGetProgramiv response is of length %lu, value=%d\n", data_size, *((GLint*)data));
    assert(data_size == sizeof(GLint));
    *params = ((GLint*)data)[0];
  }
}

GLAPI void glip_glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glGetProgramInfoLog is local\n");
    return glad_glGetProgramInfoLog(program, maxLength, length, infoLog);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLuint) + sizeof(GLsizei);
    char *data = (char *)malloc(data_size);
    *((uint32_t*)data) = _glip_glGetProgramInfoLog;
    *((GLuint*)(data+sizeof(uint32_t))) = program;
    *((GLsizei*)(data+sizeof(uint32_t)+sizeof(GLuint))) = maxLength;
    glip_send(_glip_current_context, data_size, data);
    free(data);
    glip_recv(_glip_current_context, &data_size, &data);
    *length = ((GLsizei*)data)[0];
    memcpy(infoLog, data+sizeof(GLsizei), *length);
    free(data);
  }
}

GLAPI void glip_glDeleteProgram(GLuint program) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glDeleteProgram is local\n");
    return glad_glDeleteProgram(program);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLuint);
    char *data = (char *)malloc(data_size);
    *((uint32_t*)data) = _glip_glDeleteProgram;
    *((GLuint*)(data+sizeof(uint32_t))) = program;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glUseProgram(GLuint program) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glUseProgram is local\n");
    return glad_glUseProgram(program);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLuint);
    char *data = (char *)malloc(data_size);
    *((uint32_t*)data) = _glip_glUseProgram;
    *((GLuint*)(data+sizeof(uint32_t))) = program;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glGetIntegerv(GLenum  pname, GLint *params) {
  if (!_glip_current_context || _glip_current_context->is_local) {
    glip_debug("glip_glGetIntegerv is local\n");
    return glad_glGetIntegerv(pname, params);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLenum);
    char *data = (char *)malloc(data_size);
    *((uint32_t*)data) = _glip_glGetIntegerv;
    *((GLenum*)(data+sizeof(uint32_t))) = pname;
    glip_send(_glip_current_context, data_size, data);
    free(data);
    glip_recv(_glip_current_context, &data_size, &data);
    glip_debug("glip_glGetIntegerv response is of length %lu, value=%d\n", data_size, *((GLint*)data));
    /*assert(data_size >= sizeof(GLint));
     *params = ((GLint*)data)[0];*/
    memcpy(params, data, data_size);
  }
}

GLAPI void glip_glGenFramebuffers(GLsizei n, GLuint *ids) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glGenFramebuffers is local\n");
    return glad_glGenFramebuffers(n, ids);
  } else {
    int i;
    size_t data_size = sizeof(uint32_t) + sizeof(GLsizei);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glGenFramebuffers;
    ((GLsizei*)(data+sizeof(uint32_t)))[0] = n;
    glip_send(_glip_current_context, data_size, data);
    free(data);
    glip_recv(_glip_current_context, &data_size, &data);
    assert(data_size == n*sizeof(GLuint));
    for (i=0; i<n; i++) {
      ids[i] = ((GLuint *)data)[i];
    }
  }
}

GLAPI void glip_glBindFramebuffer(GLenum target, GLuint framebuffer) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glBindFramebuffer is local\n");
    return glad_glBindFramebuffer(target, framebuffer);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLenum) + sizeof(GLuint);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glBindFramebuffer;
    ((GLenum*)(data+sizeof(uint32_t)))[0] = target;
    ((GLuint*)(data+sizeof(uint32_t)+sizeof(GLenum)))[0] = framebuffer;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glGenTextures(GLsizei n, GLuint * textures) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glGenTextures is local\n");
    return glad_glGenTextures(n, textures);
  } else {
    int i;
    size_t data_size = sizeof(uint32_t) + sizeof(GLsizei);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glGenTextures;
    ((GLsizei*)(data+sizeof(uint32_t)))[0] = n;
    glip_send(_glip_current_context, data_size, data);
    free(data);
    glip_recv(_glip_current_context, &data_size, &data);
    assert(data_size == n*sizeof(GLuint));
    for (i=0; i<n; i++) {
      textures[i] = ((GLuint *)data)[i];
    }
  }
}

GLAPI void glip_glTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * gl_data) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glTexImage2D is local\n");
    return glad_glTexImage2D(target, level, internalFormat, width, height, border, format, type, gl_data);
  } else {
    GLuint bytes_per_pixel = 0;
    switch (target) {
      case GL_PROXY_TEXTURE_2D:
      case GL_PROXY_TEXTURE_1D_ARRAY:
      case GL_PROXY_TEXTURE_CUBE_MAP:
      case GL_PROXY_TEXTURE_RECTANGLE:
        gl_data = NULL; // no data is read from target
        break;;
      case GL_TEXTURE_2D:
      case GL_TEXTURE_RECTANGLE:
      case GL_TEXTURE_CUBE_MAP:
      case GL_TEXTURE_1D_ARRAY:
        switch (type) {
          case GL_UNSIGNED_BYTE:
          case GL_BYTE:
          case GL_UNSIGNED_BYTE_3_3_2:
          case GL_UNSIGNED_BYTE_2_3_3_REV:
            bytes_per_pixel = 1;
            break;;
          case GL_UNSIGNED_SHORT:
          case GL_SHORT:
          case GL_UNSIGNED_SHORT_5_6_5:
          case GL_UNSIGNED_SHORT_5_6_5_REV:
          case GL_UNSIGNED_SHORT_4_4_4_4:
          case GL_UNSIGNED_SHORT_4_4_4_4_REV:
          case GL_UNSIGNED_SHORT_5_5_5_1:
          case GL_UNSIGNED_SHORT_1_5_5_5_REV:
            bytes_per_pixel = 2;
            break;;
          case GL_UNSIGNED_INT:
          case GL_INT:
          case GL_UNSIGNED_INT_8_8_8_8:
          case GL_UNSIGNED_INT_8_8_8_8_REV:
          case GL_UNSIGNED_INT_10_10_10_2:
          case GL_UNSIGNED_INT_2_10_10_10_REV:
            bytes_per_pixel = 4;
            break;;
          case GL_FLOAT:
            bytes_per_pixel = 4;
            break;;
          default:
            fprintf(stderr, "error: unimplemented type in glip_glTexImage2D for target GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE or GL_TEXTURE_CUBE_MAP\n");
        }
        break;;
      default:
        fprintf(stderr, "error: unimplemented target in glip_glTexImage2D\n");
    }
    GLsizei data_length = 0;
    if (gl_data && bytes_per_pixel) {
      GLint align;
      glip_glGetIntegerv(GL_UNPACK_ALIGNMENT, &align);
      data_length = (((width * bytes_per_pixel)+align-1)/align)*align * height;
    }
    size_t data_size = sizeof(uint32_t) + 3*sizeof(GLenum) + 3*sizeof(GLint) + 3*sizeof(GLsizei) + data_length; // 1 GLsizei added for data length
    char *data = (char *)malloc(data_size);
    GLsizei offset = sizeof(uint32_t);
    ((uint32_t*)data)[0] = _glip_glTexImage2D;
    *((GLenum*)(data+offset)) = target;
    offset += sizeof(GLenum);
    *((GLint*)(data+offset)) = level;
    offset += sizeof(GLint);
    *((GLint*)(data+offset)) = internalFormat;
    offset += sizeof(GLint);
    *((GLsizei*)(data+offset)) = width;
    offset += sizeof(GLsizei);
    *((GLsizei*)(data+offset)) = height;
    offset += sizeof(GLsizei);
    *((GLint*)(data+offset)) = border;
    offset += sizeof(GLint);
    *((GLenum*)(data+offset)) = format;
    offset += sizeof(GLenum);
    *((GLenum*)(data+offset)) = type;
    offset += sizeof(GLenum);
    *((GLsizei*)(data+offset)) = data_length;
    offset += sizeof(GLsizei);
    memcpy(data+offset, gl_data, data_length);
    
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glTexParameteri(GLenum target, GLenum pname, GLint param) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glTexParameteri is local\n");
    return glad_glTexParameteri(target, pname, param);
  } else {
    size_t data_size = sizeof(uint32_t) + 2*sizeof(GLenum) + sizeof(GLuint);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glTexParameteri;
    ((GLenum*)(data+sizeof(uint32_t)))[0] = target;
    ((GLenum*)(data+sizeof(uint32_t)+sizeof(GLenum)))[0] = pname;
    ((GLint*)(data+sizeof(uint32_t)+2*sizeof(GLenum)))[0] = param;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glFramebufferTexture(GLenum target, GLenum attachment, GLuint texture, GLint level) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glFramebufferTexture is local\n");
    return glad_glFramebufferTexture(target, attachment, texture, level);
  } else {
    size_t data_size = sizeof(uint32_t) + 2*sizeof(GLenum) + sizeof(GLuint) + sizeof(GLint);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glFramebufferTexture;
    ((GLenum*)(data+sizeof(uint32_t)))[0] = target;
    ((GLenum*)(data+sizeof(uint32_t)+sizeof(GLenum)))[0] = attachment;
    ((GLuint*)(data+sizeof(uint32_t)+2*sizeof(GLenum)))[0] = texture;
    ((GLint*)(data+sizeof(uint32_t)+2*sizeof(GLenum)+sizeof(GLuint)))[0] = level;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glBindTexture(GLenum target, GLuint texture) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glBindTexture is local\n");
    return glad_glBindTexture(target, texture);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLenum) + sizeof(GLuint);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glBindTexture;
    ((GLenum*)(data+sizeof(uint32_t)))[0] = target;
    ((GLuint*)(data+sizeof(uint32_t)+sizeof(GLenum)))[0] = texture;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glDrawBuffers(GLsizei n, const GLenum *bufs) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glDrawBuffers is local\n");
    return glad_glDrawBuffers(n, bufs);
  } else {
    int i;
    size_t data_size = sizeof(uint32_t) + sizeof(GLsizei) + n*sizeof(GLenum);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glDrawBuffers;
    ((GLsizei*)(data+sizeof(uint32_t)))[0] = n;
    for (i=0; i<n; i++) {
      ((GLenum*)(data+sizeof(uint32_t)+sizeof(GLsizei)))[i] = bufs[i];
    }
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glGenBuffers(GLsizei n, GLuint * buffers) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glGenBuffers is local\n");
    return glad_glGenBuffers(n, buffers);
  } else {
    int i;
    size_t data_size = sizeof(uint32_t) + sizeof(GLsizei);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glGenBuffers;
    ((GLsizei*)(data+sizeof(uint32_t)))[0] = n;
    glip_send(_glip_current_context, data_size, data);
    free(data);
    glip_recv(_glip_current_context, &data_size, &data);
    assert(data_size == n*sizeof(GLuint));
    for (i=0; i<n; i++) {
      buffers[i] = ((GLuint *)data)[i];
    }
  }
}

GLAPI void glip_glBindBuffer(GLenum target, GLuint buffer) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glBindBuffer is local\n");
    return glad_glBindBuffer(target, buffer);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLenum) + sizeof(GLuint);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glBindBuffer;
    ((GLenum*)(data+sizeof(uint32_t)))[0] = target;
    ((GLuint*)(data+sizeof(uint32_t)+sizeof(GLenum)))[0] = buffer;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glBufferData(GLenum target, GLsizeiptr size, const GLvoid * gl_data, GLenum usage) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glBufferData is local\n");
    return glad_glBufferData(target, size, gl_data, usage);
  } else {
    size_t data_size = sizeof(uint32_t) + 2*sizeof(GLenum) + sizeof(GLsizeiptr) + size;
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glBufferData;
    GLsizei offset = sizeof(uint32_t);
    ((GLenum*)(data+offset))[0] = target;
    offset += sizeof(GLenum);
    ((GLsizeiptr*)(data+offset))[0] = size;
    offset += sizeof(GLsizeiptr);
    memcpy(data+offset, gl_data, size);
    offset += size;
    ((GLenum*)(data+offset))[0] = usage;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glGenVertexArrays(GLsizei n, GLuint *arrays) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glGenVertexArrays is local\n");
    return glad_glGenVertexArrays(n, arrays);
  } else {
    int i;
    size_t data_size = sizeof(uint32_t) + sizeof(GLsizei);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glGenVertexArrays;
    ((GLsizei*)(data+sizeof(uint32_t)))[0] = n;
    glip_send(_glip_current_context, data_size, data);
    free(data);
    glip_recv(_glip_current_context, &data_size, &data);
    assert(data_size == n*sizeof(GLuint));
    for (i=0; i<n; i++) {
      arrays[i] = ((GLuint *)data)[i];
    }
  }
}

GLAPI void glip_glBindVertexArray(GLuint array) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glBindVertexArray is local\n");
    return glad_glBindVertexArray(array);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLuint);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glBindVertexArray;
    ((GLuint*)(data+sizeof(uint32_t)))[0] = array;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glVertexAttribPointer is local\n");
    return glad_glVertexAttribPointer(index, size, type, normalized, stride, pointer);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLuint) + sizeof(GLint) + sizeof(GLenum) + sizeof(GLboolean) + sizeof(GLsizei) + sizeof(void *);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glVertexAttribPointer;
    GLsizei offset = sizeof(uint32_t);
    ((GLuint*)(data+offset))[0] = index;
    offset += sizeof(GLuint);
    ((GLint*)(data+offset))[0] = size;
    offset += sizeof(GLint);
    ((GLenum*)(data+offset))[0] = type;
    offset += sizeof(GLenum);
    ((GLboolean*)(data+offset))[0] = normalized;
    offset += sizeof(GLboolean);
    ((GLsizei*)(data+offset))[0] = stride;
    offset += sizeof(GLsizei);
    memcpy(data+offset, &pointer, sizeof(void *));
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI GLenum glip_glCheckFramebufferStatus(GLenum target) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glCheckFramebufferStatus is local\n");
    return glad_glCheckFramebufferStatus(target);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLenum);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glCheckFramebufferStatus;
    ((GLenum*)(data+4))[0] = target;
    glip_send(_glip_current_context, data_size, data);
    free(data);
    glip_recv(_glip_current_context, &data_size, &data);
    assert(data_size == sizeof(GLuint));
    return ((GLuint *)data)[0];
  }
}

GLAPI GLint glip_glGetAttribLocation(GLuint program, const GLchar *name) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glGetAttribLocation is local\n");
    return glad_glGetAttribLocation(program, name);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLuint) + strlen(name)+1;
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glGetAttribLocation;
    ((GLuint*)(data+sizeof(uint32_t)))[0] = program;
    memcpy(data+sizeof(uint32_t)+sizeof(GLuint), name, strlen(name)+1);
    glip_send(_glip_current_context, data_size, data);
    free(data);
    glip_recv(_glip_current_context, &data_size, &data);
    assert(data_size == sizeof(GLint));
    return ((GLint *)data)[0];
  }
}

GLAPI void glip_glEnableVertexAttribArray(GLuint index) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glEnableVertexAttribArray is local\n");
    return glad_glEnableVertexAttribArray(index);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLuint);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glEnableVertexAttribArray;
    ((GLuint*)(data+sizeof(uint32_t)))[0] = index;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glEnable(GLenum cap) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glEnable is local\n");
    return glad_glEnable(cap);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLenum);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glEnable;
    ((GLenum*)(data+sizeof(uint32_t)))[0] = cap;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glClear(GLbitfield mask) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glClear is local\n");
    return glad_glClear(mask);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLbitfield);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glClear;
    ((GLbitfield*)(data+sizeof(uint32_t)))[0] = mask;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glViewport is local\n");
    return glad_glViewport(x, y, width, height);
  } else {
    size_t data_size = sizeof(uint32_t) + 2*sizeof(GLint) + 2*sizeof(GLsizei);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glViewport;
    ((GLint*)(data+sizeof(uint32_t)))[0] = x;
    ((GLint*)(data+sizeof(uint32_t)))[1] = y;
    ((GLsizei*)(data+sizeof(uint32_t)+2*sizeof(GLsizei)))[0] = width;
    ((GLsizei*)(data+sizeof(uint32_t)+2*sizeof(GLsizei)))[1] = height;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI GLint glip_glGetUniformLocation(GLuint program, const GLchar *name) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glGetUniformLocation is local\n");
    return glad_glGetUniformLocation(program, name);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLuint) + strlen(name)+1;
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glGetUniformLocation;
    ((GLuint*)(data+sizeof(uint32_t)))[0] = program;
    memcpy(data+sizeof(uint32_t)+sizeof(GLuint), name, strlen(name)+1);
    glip_send(_glip_current_context, data_size, data);
    free(data);
    glip_recv(_glip_current_context, &data_size, &data);
    assert(data_size == sizeof(GLint));
    return ((GLint *)data)[0];
  }
}

GLAPI void glip_glUniform4fv(GLint location, GLsizei count, const GLfloat *value) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glUniform4fv is local\n");
    return glad_glUniform4fv(location, count, value);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLint) + sizeof(GLsizei) + count*4*sizeof(GLfloat);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glUniform4fv;
    ((GLint*)(data+sizeof(uint32_t)))[0] = location;
    ((GLsizei*)(data+sizeof(uint32_t)+sizeof(GLint)))[0] = count;
    memcpy(data+sizeof(uint32_t)+sizeof(GLint)+sizeof(GLsizei), value, count*4*sizeof(GLfloat));
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glDrawArrays(GLenum mode, GLint first, GLsizei count) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glDrawArrays is local\n");
    return glad_glDrawArrays(mode, first, count);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLenum) + sizeof(GLint) + sizeof(GLsizei);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glDrawArrays;
    ((GLenum*)(data+sizeof(uint32_t)))[0] = mode;
    ((GLint*)(data+sizeof(uint32_t)+sizeof(GLenum)))[0] = first;
    ((GLsizei*)(data+sizeof(uint32_t)+sizeof(GLenum)+sizeof(GLint)))[0] = count;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI GLenum glip_glGetError(void) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glGetError is local\n");
    return glad_glGetError();
  } else {
    size_t data_size = sizeof(uint32_t);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glGetError;
    glip_send(_glip_current_context, data_size, data);
    free(data);
    glip_recv(_glip_current_context, &data_size, &data);
    assert(data_size == sizeof(GLenum));
    return ((GLenum *)data)[0];
  }
}

GLAPI const GLubyte *glip_glGetString(GLenum name) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glGetString is local\n");
    return glad_glGetString(name);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLenum);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glGetString;
    ((GLenum*)(data+sizeof(uint32_t)))[0] = name;
    glip_send(_glip_current_context, data_size, data);
    free(data);
    glip_recv(_glip_current_context, &data_size, &data);
    return (const GLubyte *)data;
  }
}

GLAPI const GLubyte *glip_glGetStringi(GLenum name, GLuint index) {
  if (!_glip_current_context || _glip_current_context->is_local) {
    glip_debug("glip_glGetStringi is local\n");
    return glad_glGetStringi(name, index);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLenum) + sizeof(GLuint);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glGetStringi;
    ((GLenum*)(data+sizeof(uint32_t)))[0] = name;
    ((GLuint*)(data+sizeof(uint32_t)+sizeof(GLenum)))[0] = index;
    glip_send(_glip_current_context, data_size, data);
    free(data);
    glip_recv(_glip_current_context, &data_size, &data);
    return (const GLubyte *)data;
  }
}

GLAPI void glip_glCullFace(GLenum mode) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glCullFace is local\n");
    return glad_glCullFace(mode);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLenum);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glCullFace;
    ((GLenum*)(data+sizeof(uint32_t)))[0] = mode;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glGenRenderbuffers(GLsizei n, GLuint *renderbuffers) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glGenRenderbuffers is local\n");
    return glad_glGenRenderbuffers(n, renderbuffers);
  } else {
    int i;
    size_t data_size = sizeof(uint32_t) + sizeof(GLsizei);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glGenRenderbuffers;
    ((GLsizei*)(data+sizeof(uint32_t)))[0] = n;
    glip_send(_glip_current_context, data_size, data);
    free(data);
    glip_recv(_glip_current_context, &data_size, &data);
    assert(data_size == n*sizeof(GLuint));
    for (i=0; i<n; i++) {
      renderbuffers[i] = ((GLuint *)data)[i];
    }
  }
}

GLAPI void glip_glBindRenderbuffer(GLenum target, GLuint renderbuffer) {
  if (_glip_current_context->is_local) {
    glip_debug("glBindRenderbuffer is local\n");
    return glad_glBindRenderbuffer(target, renderbuffer);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLenum) + sizeof(GLuint);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glBindRenderbuffer;
    ((GLenum*)(data+sizeof(uint32_t)))[0] = target;
    ((GLuint*)(data+sizeof(uint32_t)+sizeof(GLenum)))[0] = renderbuffer;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glRenderbufferStorage is local\n");
    return glad_glRenderbufferStorage(target, internalformat, width, height);
  } else {
    size_t data_size = sizeof(uint32_t) + 2*sizeof(GLenum) + 2*sizeof(GLsizei);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glRenderbufferStorage;
    ((GLenum*)(data+sizeof(uint32_t)))[0] = target;
    ((GLenum*)(data+sizeof(uint32_t)))[1] = internalformat;
    ((GLsizei*)(data+sizeof(uint32_t)+2*sizeof(GLenum)))[0] = width;
    ((GLsizei*)(data+sizeof(uint32_t)+2*sizeof(GLenum)))[1] = height;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glFramebufferRenderbuffer is local\n");
    return glad_glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
  } else {
    size_t data_size = sizeof(uint32_t) + 3*sizeof(GLenum) + sizeof(GLuint);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glFramebufferRenderbuffer;
    ((GLenum*)(data+sizeof(uint32_t)))[0] = target;
    ((GLenum*)(data+sizeof(uint32_t)))[1] = attachment;
    ((GLenum*)(data+sizeof(uint32_t)))[2] = renderbuffertarget;
    ((GLsizei*)(data+sizeof(uint32_t)+3*sizeof(GLenum)))[0] = renderbuffer;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glVertexAttribDivisor(GLuint index, GLuint divisor) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glVertexAttribDivisor is local\n");
    return glad_glVertexAttribDivisor(index, divisor);
  } else {
    size_t data_size = sizeof(uint32_t) + 2*sizeof(GLuint);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glVertexAttribDivisor;
    ((GLuint*)(data+sizeof(uint32_t)))[0] = index;
    ((GLuint*)(data+sizeof(uint32_t)))[1] = divisor;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void * indices, GLsizei primcount) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glDrawElementsInstanced is local\n");
    return glad_glDrawElementsInstanced(mode, count, type, indices, primcount);
  } else {
    GLint elem_buffer_bind;
    glip_glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elem_buffer_bind);
    size_t data_size, type_size;
    switch (type) {
      case GL_UNSIGNED_BYTE:
        type_size = 1;
        break;
      case GL_UNSIGNED_SHORT:
        type_size = 2;
        break;
      case GL_UNSIGNED_INT:
        type_size = 4;
        break;
      default:
        fprintf(stderr, "Unallowed type in glDrawElementsInstanced\n");
        return;
    }
    if (elem_buffer_bind) {
      data_size = sizeof(uint32_t) + 2*sizeof(GLenum) + 2*sizeof(GLsizei) + sizeof(GLboolean) + sizeof(void *);
    } else {
      data_size = sizeof(uint32_t) + 2*sizeof(GLenum) + 2*sizeof(GLsizei) + sizeof(GLboolean) + type_size*count;
    }
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glDrawElementsInstanced;
    ((GLenum*)(data+sizeof(uint32_t)))[0] = mode;
    ((GLsizei*)(data+sizeof(uint32_t)+sizeof(GLenum)))[0] = count;
    ((GLenum*)(data+sizeof(uint32_t)+sizeof(GLenum)+sizeof(GLsizei)))[0] = type;
    ((GLsizei*)(data+sizeof(uint32_t)+2*sizeof(GLenum)+sizeof(GLsizei)))[0] = primcount;
    ((GLboolean*)(data+sizeof(uint32_t)+2*sizeof(GLenum)+2*sizeof(GLsizei)))[0] = elem_buffer_bind;
    if (elem_buffer_bind) {
      memcpy(data+sizeof(uint32_t)+2*sizeof(GLenum)+2*sizeof(GLsizei)+sizeof(GLboolean), &indices, sizeof(void *));
    } else {
      memcpy(data+sizeof(uint32_t)+2*sizeof(GLenum)+2*sizeof(GLsizei)+sizeof(GLboolean), indices, type_size*count);
    }
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei primcount) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glDrawArraysInstanced is local\n");
    return glad_glDrawArraysInstanced(mode, first, count, primcount);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLenum) + sizeof(GLint) + 2*sizeof(GLsizei);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glDrawArraysInstanced;
    ((GLenum*)(data+sizeof(uint32_t)))[0] = mode;
    ((GLint*)(data+sizeof(uint32_t)+sizeof(GLenum)))[0] = first;
    ((GLsizei*)(data+sizeof(uint32_t)+sizeof(GLenum)+sizeof(GLint)))[0] = count;
    ((GLsizei*)(data+sizeof(uint32_t)+sizeof(GLenum)+sizeof(GLint)))[1] = primcount;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glDisable(GLenum cap) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glDisable is local\n");
    return glad_glDisable(cap);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLenum);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glDisable;
    ((GLenum*)(data+sizeof(uint32_t)))[0] = cap;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glUniform1f(GLint location, GLfloat v0) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glUniform1f is local\n");
    return glad_glUniform1f(location, v0);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLint) + sizeof(GLfloat);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glUniform1f;
    ((GLint*)(data+sizeof(uint32_t)))[0] = location;
    ((GLfloat*)(data+sizeof(uint32_t)+sizeof(GLint)))[0] = v0;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glUniform2f(GLint location, GLfloat v0, GLfloat v1){
  if (_glip_current_context->is_local) {
    glip_debug("glip_glUniform2f is local\n");
    return glad_glUniform2f(location, v0, v1);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLint) + 2*sizeof(GLfloat);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glUniform2f;
    ((GLint*)(data+sizeof(uint32_t)))[0] = location;
    ((GLfloat*)(data+sizeof(uint32_t)+sizeof(GLint)))[0] = v0;
    ((GLfloat*)(data+sizeof(uint32_t)+sizeof(GLint)))[1] = v1;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2){
  if (_glip_current_context->is_local) {
    glip_debug("glip_glUniform3f is local\n");
    return glad_glUniform3f(location, v0, v1, v2);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLint) + 3*sizeof(GLfloat);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glUniform3f;
    ((GLint*)(data+sizeof(uint32_t)))[0] = location;
    ((GLfloat*)(data+sizeof(uint32_t)+sizeof(GLint)))[0] = v0;
    ((GLfloat*)(data+sizeof(uint32_t)+sizeof(GLint)))[1] = v1;
    ((GLfloat*)(data+sizeof(uint32_t)+sizeof(GLint)))[2] = v2;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glUniform4f is local\n");
    return glad_glUniform4f(location, v0, v1, v2, v3);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLint) + 4*sizeof(GLfloat);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glUniform4f;
    ((GLint*)(data+sizeof(uint32_t)))[0] = location;
    ((GLfloat*)(data+sizeof(uint32_t)+sizeof(GLint)))[0] = v0;
    ((GLfloat*)(data+sizeof(uint32_t)+sizeof(GLint)))[1] = v1;
    ((GLfloat*)(data+sizeof(uint32_t)+sizeof(GLint)))[2] = v2;
    ((GLfloat*)(data+sizeof(uint32_t)+sizeof(GLint)))[3] = v3;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glUniform1fv(GLint location, GLsizei count, const GLfloat *value) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glUniform1fv is local\n");
    return glad_glUniform1fv(location, count, value);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLint) + sizeof(GLsizei) + count*1*sizeof(GLfloat);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glUniform1fv;
    ((GLint*)(data+sizeof(uint32_t)))[0] = location;
    ((GLsizei*)(data+sizeof(uint32_t)+sizeof(GLint)))[0] = count;
    memcpy(data+sizeof(uint32_t)+sizeof(GLint)+sizeof(GLsizei), value, count*1*sizeof(GLfloat));
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glUniform2fv(GLint location, GLsizei count, const GLfloat *value) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glUniform2fv is local\n");
    return glad_glUniform2fv(location, count, value);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLint) + sizeof(GLsizei) + count*2*sizeof(GLfloat);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glUniform2fv;
    ((GLint*)(data+sizeof(uint32_t)))[0] = location;
    ((GLsizei*)(data+sizeof(uint32_t)+sizeof(GLint)))[0] = count;
    memcpy(data+sizeof(uint32_t)+sizeof(GLint)+sizeof(GLsizei), value, count*2*sizeof(GLfloat));
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glUniform3fv(GLint location, GLsizei count, const GLfloat *value) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glUniform3fv is local\n");
    return glad_glUniform3fv(location, count, value);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLint) + sizeof(GLsizei) + count*3*sizeof(GLfloat);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glUniform3fv;
    ((GLint*)(data+sizeof(uint32_t)))[0] = location;
    ((GLsizei*)(data+sizeof(uint32_t)+sizeof(GLint)))[0] = count;
    memcpy(data+sizeof(uint32_t)+sizeof(GLint)+sizeof(GLsizei), value, count*3*sizeof(GLfloat));
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glUniformMatrix2fv is local\n");
    return glad_glUniformMatrix2fv(location, count, transpose, value);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLint) + sizeof(GLsizei) + sizeof(GLboolean) + count*2*2*sizeof(GLfloat);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glUniformMatrix2fv;
    ((GLint*)(data+sizeof(uint32_t)))[0] = location;
    ((GLsizei*)(data+sizeof(uint32_t)+sizeof(GLint)))[0] = count;
    ((GLboolean*)(data+sizeof(uint32_t)+sizeof(GLint)+sizeof(GLsizei)))[0] = transpose;
    memcpy(data+sizeof(uint32_t)+sizeof(GLint)+sizeof(GLsizei)+sizeof(GLboolean), value, count*2*2*sizeof(GLfloat));
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glUniformMatrix3fv is local\n");
    return glad_glUniformMatrix3fv(location, count, transpose, value);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLint) + sizeof(GLsizei) + sizeof(GLboolean) + count*3*3*sizeof(GLfloat);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glUniformMatrix3fv;
    ((GLint*)(data+sizeof(uint32_t)))[0] = location;
    ((GLsizei*)(data+sizeof(uint32_t)+sizeof(GLint)))[0] = count;
    ((GLboolean*)(data+sizeof(uint32_t)+sizeof(GLint)+sizeof(GLsizei)))[0] = transpose;
    memcpy(data+sizeof(uint32_t)+sizeof(GLint)+sizeof(GLsizei)+sizeof(GLboolean), value, count*3*3*sizeof(GLfloat));
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glUniformMatrix4fv is local\n");
    return glad_glUniformMatrix4fv(location, count, transpose, value);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLint) + sizeof(GLsizei) + sizeof(GLboolean) + count*4*4*sizeof(GLfloat);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glUniformMatrix4fv;
    ((GLint*)(data+sizeof(uint32_t)))[0] = location;
    ((GLsizei*)(data+sizeof(uint32_t)+sizeof(GLint)))[0] = count;
    ((GLboolean*)(data+sizeof(uint32_t)+sizeof(GLint)+sizeof(GLsizei)))[0] = transpose;
    memcpy(data+sizeof(uint32_t)+sizeof(GLint)+sizeof(GLsizei)+sizeof(GLboolean), value, count*4*4*sizeof(GLfloat));
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glUniform1i(GLint location, GLint v0) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glUniform1i is local\n");
    return glad_glUniform1i(location, v0);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLint) + sizeof(GLint);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glUniform1i;
    ((GLint*)(data+sizeof(uint32_t)))[0] = location;
    ((GLfloat*)(data+sizeof(uint32_t)+sizeof(GLint)))[0] = v0;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glUniform2i(GLint location, GLint v0, GLint v1){
  if (_glip_current_context->is_local) {
    glip_debug("glip_glUniform2i is local\n");
    return glad_glUniform2i(location, v0, v1);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLint) + 2*sizeof(GLint);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glUniform2i;
    ((GLint*)(data+sizeof(uint32_t)))[0] = location;
    ((GLfloat*)(data+sizeof(uint32_t)+sizeof(GLint)))[0] = v0;
    ((GLfloat*)(data+sizeof(uint32_t)+sizeof(GLint)))[1] = v1;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glUniform3i(GLint location, GLint v0, GLint v1, GLint v2){
  if (_glip_current_context->is_local) {
    glip_debug("glip_glUniform3i is local\n");
    return glad_glUniform3i(location, v0, v1, v2);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLint) + 3*sizeof(GLint);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glUniform3i;
    ((GLint*)(data+sizeof(uint32_t)))[0] = location;
    ((GLfloat*)(data+sizeof(uint32_t)+sizeof(GLint)))[0] = v0;
    ((GLfloat*)(data+sizeof(uint32_t)+sizeof(GLint)))[1] = v1;
    ((GLfloat*)(data+sizeof(uint32_t)+sizeof(GLint)))[2] = v2;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glUniform4i is local\n");
    return glad_glUniform4i(location, v0, v1, v2, v3);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLint) + 4*sizeof(GLint);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glUniform4i;
    ((GLint*)(data+sizeof(uint32_t)))[0] = location;
    ((GLfloat*)(data+sizeof(uint32_t)+sizeof(GLint)))[0] = v0;
    ((GLfloat*)(data+sizeof(uint32_t)+sizeof(GLint)))[1] = v1;
    ((GLfloat*)(data+sizeof(uint32_t)+sizeof(GLint)))[2] = v2;
    ((GLfloat*)(data+sizeof(uint32_t)+sizeof(GLint)))[3] = v3;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glActiveTexture(GLenum texture) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glActiveTexture is local\n");
    return glad_glActiveTexture(texture);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLenum);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glActiveTexture;
    ((GLenum*)(data+sizeof(uint32_t)))[0] = texture;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glClearColor is local\n");
    return glad_glClearColor(red, green, blue, alpha);
  } else {
    size_t data_size = sizeof(uint32_t) + 4*sizeof(GLfloat);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glClearColor;
    ((GLfloat*)(data+sizeof(uint32_t)))[0] = red;
    ((GLfloat*)(data+sizeof(uint32_t)))[1] = green;
    ((GLfloat*)(data+sizeof(uint32_t)))[2] = blue;
    ((GLfloat*)(data+sizeof(uint32_t)))[3] = alpha;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glTexParameterf(GLenum target, GLenum pname, GLfloat param) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glTexParameterf is local\n");
    return glad_glTexParameterf(target, pname, param);
  } else {
    size_t data_size = sizeof(uint32_t) + 2*sizeof(GLenum) + sizeof(GLfloat);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glTexParameterf;
    ((GLenum*)(data+sizeof(uint32_t)))[0] = target;
    ((GLenum*)(data+sizeof(uint32_t)+sizeof(GLenum)))[0] = pname;
    ((GLfloat*)(data+sizeof(uint32_t)+2*sizeof(GLenum)))[0] = param;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glGenerateMipmap(GLenum target) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glGenerateMipmap is local\n");
    return glad_glGenerateMipmap(target);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLenum);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glGenerateMipmap;
    ((GLenum*)(data+sizeof(uint32_t)))[0] = target;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glDeleteBuffers(GLsizei n, const GLuint *buffers) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glDeleteBuffers is local\n");
    return glad_glDeleteBuffers(n, buffers);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLsizei) + n*sizeof(GLuint);
    char *data = (char *)malloc(data_size);
    *((uint32_t*)data) = _glip_glDeleteBuffers;
    *((GLsizei*)(data+sizeof(uint32_t))) = n;
    memcpy(data+sizeof(uint32_t)+sizeof(GLsizei), buffers, n*sizeof(GLuint));
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glDeleteVertexArrays(GLsizei n, const GLuint * arrays) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glDeleteVertexArrays is local\n");
    return glad_glDeleteVertexArrays(n, arrays);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLsizei) + n*sizeof(GLuint);
    char *data = (char *)malloc(data_size);
    *((uint32_t*)data) = _glip_glDeleteVertexArrays;
    *((GLsizei*)(data+sizeof(uint32_t))) = n;
    memcpy(data+sizeof(uint32_t)+sizeof(GLsizei), arrays, n*sizeof(GLuint));
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glDeleteTextures(GLsizei n, const GLuint * textures) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glDeleteTextures is local\n");
    return glad_glDeleteTextures(n, textures);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLsizei) + n*sizeof(GLuint);
    char *data = (char *)malloc(data_size);
    *((uint32_t*)data) = _glip_glDeleteTextures;
    *((GLsizei*)(data+sizeof(uint32_t))) = n;
    memcpy(data+sizeof(uint32_t)+sizeof(GLsizei), textures, n*sizeof(GLuint));
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glDeleteFramebuffers(GLsizei n, const GLuint * framebuffers) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glDeleteFramebuffers is local\n");
    return glad_glDeleteFramebuffers(n, framebuffers);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLsizei) + n*sizeof(GLuint);
    char *data = (char *)malloc(data_size);
    *((uint32_t*)data) = _glip_glDeleteFramebuffers;
    *((GLsizei*)(data+sizeof(uint32_t))) = n;
    memcpy(data+sizeof(uint32_t)+sizeof(GLsizei), framebuffers, n*sizeof(GLuint));
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glReadBuffer(GLenum mode) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glReadBuffer is local\n");
    return glad_glReadBuffer(mode);
  } else {
    size_t data_size = sizeof(uint32_t) + sizeof(GLenum);
    char *data = (char *)malloc(data_size);
    *((uint32_t*)data) = _glip_glReadBuffer;
    *((GLenum*)(data+sizeof(uint32_t))) = mode;
    glip_send(_glip_current_context, data_size, data);
    free(data);
  }
}

GLAPI void glip_glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid * gl_data) {
  if (_glip_current_context->is_local) {
    glip_debug("glip_glReadPixels is local\n");
    return glad_glReadPixels(x, y, width, height, format, type, gl_data);
  } else {
    size_t data_size = 2*sizeof(GLint) + 2*sizeof(GLsizei) + 2*sizeof(GLenum);
    char *data = (char *)malloc(data_size);
    ((uint32_t*)data)[0] = _glip_glReadPixels;
    ((GLint*)(data+sizeof(uint32_t)))[0] = x;
    ((GLint*)(data+sizeof(uint32_t)))[1] = y;
    ((GLsizei*)(data+sizeof(uint32_t)+2*sizeof(GLint)))[0] = width;
    ((GLsizei*)(data+sizeof(uint32_t)+2*sizeof(GLint)))[1] = height;
    ((GLenum*)(data+sizeof(uint32_t)+2*sizeof(GLint)+2*sizeof(GLsizei)))[0] = format;
    ((GLenum*)(data+sizeof(uint32_t)+2*sizeof(GLint)+2*sizeof(GLsizei)))[1] = type;
    glip_send(_glip_current_context, data_size, data);
    free(data);
    glip_recv(_glip_current_context, &data_size, &data);
    assert(data_size == sizeof(GLint));
    memcpy(gl_data, data, data_size);
    free(data);
  }
}
