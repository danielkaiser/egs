/*
//  glip_impl.c
//  egs
//
//  Created by Daniel Kaiser on 01/07/16.
//  Copyright © 2016 Daniel Kaiser. All rights reserved.
*/

#include "glip_impl.h"
#include "glad.h"
#include "glip.h"
#include <stdlib.h>
#include <string.h>

void glip_impl_glCreateShader(const char *data, char** res_data, size_t *res_len) {
  *res_len = sizeof(GLuint);
  *res_data = (char *)malloc(*res_len);
  GLuint shader = glad_glCreateShader(((GLenum *)data)[0]);
  glip_debug("shader is %d\n",shader);
  memcpy(*res_data, &shader, sizeof(GLuint));
  glip_debug("after memcpy\n");
}

void glip_impl_glShaderSource(const char *data, char** res_data, size_t *res_len) {
  int i;
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glShaderSource\n");
  GLuint shader = ((GLuint*)data)[0];
  GLsizei count = ((GLsizei*)(data+sizeof(GLuint)))[0];
  data += sizeof(GLuint) + sizeof(GLsizei);
  char **strings = (char**)malloc(count*sizeof(char *));
  GLint* length = (GLint*)malloc(sizeof(GLint)*count);
  for (i=0; i<count; i++) {
    length[i] = *((GLint *)data);
    strings[i] = (char *)malloc(length[i]);
    data += sizeof(GLint);
  }
  for (i=0; i<count; i++) {
    memcpy(strings[i], ((char *)data), length[i]);
    data += length[i];
  }
  glad_glShaderSource(shader, count, (const char**)strings, length);
  for (i=0; i<count; i++) {
    free(strings[i]);
  }
  free(strings);
  free(length);
}

void glip_impl_glCompileShader(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glCompileShader\n");
  glip_debug("shader: %d\n", ((GLuint *)data)[0]);
  glad_glCompileShader(((GLuint *)data)[0]);
}

void glip_impl_glGetShaderiv(const char *data, char** res_data, size_t *res_len) {
  *res_len = sizeof(GLint);
  *res_data = (char *)malloc(*res_len);
  glip_debug("glip_impl_glGetShaderiv\n");
  GLuint shader = *((GLuint *)data);
  GLenum pname = *((GLenum *)(data+sizeof(GLuint)));
  glad_glGetShaderiv(shader, pname, (GLint*)(*res_data));
  glip_debug("-----> result: %d\n", *((GLint*)*res_data));
}

void glip_impl_glGetShaderInfoLog(const char *data, char** res_data, size_t *res_len) {
  glip_debug("glip_impl_glGetShaderInfoLog\n");
  GLuint shader = *((GLuint *)data);
  GLsizei maxLength = *((GLsizei *)(data+sizeof(GLuint)));
  glip_debug("maxlength is %d\n", maxLength);
  *res_len = sizeof(GLsizei) + maxLength;
  *res_data = (char *)malloc(*res_len);
  glad_glGetShaderInfoLog(shader, maxLength, (GLsizei*)(*res_data), (*res_data)+sizeof(GLsizei));
}

void glip_impl_glDeleteShader(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glDeleteShader\n");
  GLuint shader = *((GLuint *)data);
  glad_glDeleteShader(shader);
}

void glip_impl_glCreateProgram(const char *data, char** res_data, size_t *res_len) {
  (void) data;
  glip_debug("glip_impl_glCreateProgram\n");
  *res_len = sizeof(GLuint);
  *res_data = (char *)malloc(*res_len);
  GLuint program = glad_glCreateProgram();
  glip_debug("program is %d\n",program);
  memcpy(*res_data, &program, sizeof(GLuint));
}

void glip_impl_glAttachShader(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glAttachShader\n");
  GLuint program = *((GLuint *)data);
  GLuint shader = *((GLuint *)(data+sizeof(GLuint)));
  glad_glAttachShader(program, shader);
}

void glip_impl_glBindFragDataLocation(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glBindFragDataLocation\n");
  GLuint program = *((GLuint *)data);
  GLuint colorNumber = *((GLuint *)(data+sizeof(GLuint)));
  glad_glBindFragDataLocation(program, colorNumber, data+2*sizeof(GLuint));
}

void glip_impl_glLinkProgram(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glLinkProgram\n");
  GLuint program = *((GLuint *)data);
  glad_glLinkProgram(program);
}

void glip_impl_glGetProgramiv(const char *data, char** res_data, size_t *res_len) {
  *res_len = sizeof(GLint);
  *res_data = (char *)malloc(*res_len);
  glip_debug("glip_impl_glGetProgramiv\n");
  GLuint program = *((GLuint *)data);
  GLenum pname = *((GLenum *)(data+sizeof(GLuint)));
  glad_glGetProgramiv(program, pname, (GLint*)(*res_data));
  glip_debug("-----> result: %d\n", *((GLint*)*res_data));
}

void glip_impl_glGetProgramInfoLog(const char *data, char** res_data, size_t *res_len) {
  glip_debug("glip_impl_glGetProgramInfoLog\n");
  GLuint program = *((GLuint *)data);
  GLsizei maxLength = *((GLsizei *)(data+sizeof(GLuint)));
  glip_debug("maxlength is %d\n", maxLength);
  *res_len = sizeof(GLsizei) + maxLength;
  *res_data = (char *)malloc(*res_len);
  glad_glGetProgramInfoLog(program, maxLength, (GLsizei*)(*res_data), (*res_data)+sizeof(GLsizei));
}

void glip_impl_glDeleteProgram(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glDeleteProgram\n");
  GLuint program = *((GLuint *)data);
  glad_glDeleteProgram(program);
}

void glip_impl_glUseProgram(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glUseProgram\n");
  GLuint program = *((GLuint *)data);
  glad_glUseProgram(program);
}

void glip_impl_glGetIntegerv(const char *data, char** res_data, size_t *res_len) {
  glip_debug("glip_impl_glGetIntegerv\n");
  GLenum pname = *((GLenum *)data);
  switch (pname) {
    case GL_ACTIVE_TEXTURE:
    case GL_ARRAY_BUFFER_BINDING:
    case GL_BLEND:
    case GL_BLEND_DST_ALPHA:
    case GL_BLEND_DST_RGB:
    case GL_BLEND_EQUATION_RGB:
    case GL_FUNC_REVERSE_SUBTRACT:
    case GL_BLEND_SRC_RGB:
    case GL_COLOR_LOGIC_OP:
    case GL_CONTEXT_FLAGS:
    case GL_CULL_FACE:
    case GL_CURRENT_PROGRAM:
    case GL_DEPTH_CLEAR_VALUE:
    case GL_DEPTH_FUNC:
    case GL_DEPTH_TEST:
    case GL_DEPTH_WRITEMASK:
    case GL_DITHER:
    case GL_DOUBLEBUFFER:
    case GL_DRAW_BUFFER:
    case GL_DRAW_FRAMEBUFFER_BINDING:
    case GL_READ_FRAMEBUFFER_BINDING:
    case GL_ELEMENT_ARRAY_BUFFER_BINDING:
    case GL_FRAGMENT_SHADER_DERIVATIVE_HINT:
    case GL_LINE_SMOOTH:
    case GL_LINE_SMOOTH_HINT:
    case GL_LINE_WIDTH:
    case GL_LOGIC_OP_MODE:
    case GL_MAJOR_VERSION:
    case GL_MAX_3D_TEXTURE_SIZE:
    case GL_MAX_ARRAY_TEXTURE_LAYERS:
    case GL_MAX_CLIP_DISTANCES:
    case GL_MAX_COLOR_TEXTURE_SAMPLES:
    case GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS:
    case GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS:
    case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS:
    case GL_MAX_COMBINED_UNIFORM_BLOCKS:
    case GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS:
    case GL_MAX_CUBE_MAP_TEXTURE_SIZE:
    case GL_MAX_DEPTH_TEXTURE_SAMPLES:
    case GL_MAX_DRAW_BUFFERS:
    case GL_MAX_DUAL_SOURCE_DRAW_BUFFERS:
    case GL_MAX_ELEMENTS_INDICES:
    case GL_MAX_ELEMENTS_VERTICES:
    case GL_MAX_FRAGMENT_INPUT_COMPONENTS:
    case GL_MAX_FRAGMENT_UNIFORM_COMPONENTS:
    case GL_MAX_GEOMETRY_INPUT_COMPONENTS:
    case GL_MAX_GEOMETRY_OUTPUT_COMPONENTS:
    case GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS:
    case GL_MAX_GEOMETRY_UNIFORM_BLOCKS:
    case GL_MAX_GEOMETRY_UNIFORM_COMPONENTS:
    case GL_MAX_INTEGER_SAMPLES:
    case GL_MAX_PROGRAM_TEXEL_OFFSET:
    case GL_MIN_PROGRAM_TEXEL_OFFSET:
    case GL_MAX_RECTANGLE_TEXTURE_SIZE:
    case GL_MAX_RENDERBUFFER_SIZE:
    case GL_MAX_SAMPLE_MASK_WORDS:
    case GL_MAX_SERVER_WAIT_TIMEOUT:
    case GL_MAX_TEXTURE_BUFFER_SIZE:
    case GL_MAX_TEXTURE_IMAGE_UNITS:
    case GL_MAX_TEXTURE_LOD_BIAS:
    case GL_MAX_TEXTURE_SIZE:
    case GL_MAX_UNIFORM_BUFFER_BINDINGS:
    case GL_MAX_UNIFORM_BLOCK_SIZE:
    case GL_MAX_VARYING_COMPONENTS:
    case GL_MAX_VERTEX_ATTRIBS:
    case GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS:
    case GL_MAX_VERTEX_UNIFORM_COMPONENTS:
    case GL_MAX_VERTEX_OUTPUT_COMPONENTS:
    case GL_MAX_VERTEX_UNIFORM_BLOCKS:
    case GL_MINOR_VERSION:
    case GL_NUM_COMPRESSED_TEXTURE_FORMATS:
    case GL_NUM_EXTENSIONS:
    case GL_PACK_ALIGNMENT:
    case GL_PACK_IMAGE_HEIGHT:
    case GL_PACK_LSB_FIRST:
    case GL_PACK_ROW_LENGTH:
    case GL_PACK_SKIP_IMAGES:
    case GL_PACK_SKIP_PIXELS:
    case GL_PACK_SKIP_ROWS:
    case GL_PACK_SWAP_BYTES:
    case GL_PIXEL_PACK_BUFFER_BINDING:
    case GL_PIXEL_UNPACK_BUFFER_BINDING:
    case GL_POINT_FADE_THRESHOLD_SIZE:
    case GL_PRIMITIVE_RESTART_INDEX:
    case GL_PROGRAM_POINT_SIZE:
    case GL_PROVOKING_VERTEX:
    case GL_POINT_SIZE:
    case GL_POINT_SIZE_GRANULARITY:
    case GL_POLYGON_OFFSET_FACTOR:
    case GL_POLYGON_OFFSET_UNITS:
    case GL_POLYGON_OFFSET_FILL:
    case GL_POLYGON_OFFSET_LINE:
    case GL_POLYGON_OFFSET_POINT:
    case GL_POLYGON_SMOOTH:
    case GL_POLYGON_SMOOTH_HINT:
    case GL_READ_BUFFER:
    case GL_RENDERBUFFER_BINDING:
    case GL_SAMPLE_BUFFERS:
    case GL_SAMPLE_COVERAGE_VALUE:
    case GL_SAMPLE_COVERAGE_INVERT:
    case GL_SAMPLER_BINDING:
    case GL_SAMPLES:
    case GL_SCISSOR_TEST:
    case GL_SMOOTH_LINE_WIDTH_GRANULARITY:
    case GL_STENCIL_BACK_FAIL:
    case GL_STENCIL_BACK_FUNC:
    case GL_STENCIL_BACK_PASS_DEPTH_FAIL:
    case GL_STENCIL_BACK_PASS_DEPTH_PASS:
    case GL_STENCIL_BACK_REF:
    case GL_STENCIL_BACK_VALUE_MASK:
    case GL_STENCIL_BACK_WRITEMASK:
    case GL_STENCIL_CLEAR_VALUE:
    case GL_STENCIL_FAIL:
    case GL_STENCIL_FUNC:
    case GL_STENCIL_PASS_DEPTH_FAIL:
    case GL_STENCIL_PASS_DEPTH_PASS:
    case GL_STENCIL_REF:
    case GL_STENCIL_TEST:
    case GL_STENCIL_VALUE_MASK:
    case GL_STENCIL_WRITEMASK:
    case GL_STEREO:
    case GL_SUBPIXEL_BITS:
    case GL_TEXTURE_BINDING_1D_ARRAY:
    case GL_TEXTURE_BINDING_2D_ARRAY:
    case GL_TEXTURE_BINDING_2D_MULTISAMPLE:
    case GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY:
    case GL_TEXTURE_BINDING_BUFFER:
    case GL_TEXTURE_BINDING_CUBE_MAP:
    case GL_TEXTURE_BINDING_RECTANGLE:
    case GL_TEXTURE_COMPRESSION_HINT:
    case GL_TIMESTAMP:
    case GL_TRANSFORM_FEEDBACK_BUFFER_BINDING:
    case GL_TRANSFORM_FEEDBACK_BUFFER_START:
    case GL_TRANSFORM_FEEDBACK_BUFFER_SIZE:
    case GL_UNIFORM_BUFFER_BINDING:
    case GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT:
    case GL_UNIFORM_BUFFER_SIZE:
    case GL_UNIFORM_BUFFER_START:
    case GL_UNPACK_ALIGNMENT:
    case GL_UNPACK_IMAGE_HEIGHT:
    case GL_UNPACK_LSB_FIRST:
    case GL_UNPACK_ROW_LENGTH:
    case GL_UNPACK_SKIP_IMAGES:
    case GL_UNPACK_SKIP_PIXELS:
    case GL_UNPACK_SKIP_ROWS:
    case GL_UNPACK_SWAP_BYTES:
    case GL_VERTEX_ARRAY_BINDING:
      *res_len = sizeof(GLint);
      break;
    case GL_ALIASED_LINE_WIDTH_RANGE:
    case GL_DEPTH_RANGE:
    case GL_MAX_VIEWPORT_DIMS:
    case GL_POINT_SIZE_RANGE:
    case GL_SMOOTH_LINE_WIDTH_RANGE:
      *res_len = 2*sizeof(GLint);
      break;
    case GL_COLOR_CLEAR_VALUE:
    case GL_COLOR_WRITEMASK:
    case GL_SCISSOR_BOX:
    case GL_VIEWPORT:
      *res_len = 4*sizeof(GLint);
      break;
    default:
      fprintf(stderr, "unsopported pname in glip_glGetIntegerv\n");
      return;
  }
  *res_data = (char *)malloc(*res_len);
  glad_glGetIntegerv(pname, (GLint*)(*res_data));
  glip_debug("-----> result: %d\n", *((GLint*)*res_data));
}

void glip_impl_glGenFramebuffers(const char *data, char** res_data, size_t *res_len) {
  glip_debug("glip_impl_glGenFramebuffers\n");
  GLsizei n =((GLsizei *)data)[0];
  *res_len = n*sizeof(GLuint);
  *res_data = (char *)malloc(*res_len);
  glad_glGenFramebuffers(n, (GLuint*)*res_data);
}

void glip_impl_glBindFramebuffer(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glBindFramebuffer\n");
  GLenum target =((GLenum *)data)[0];
  GLuint framebuffer =((GLuint *)(data+sizeof(GLenum)))[0];
  glad_glBindFramebuffer(target, framebuffer);
}

void glip_impl_glGenTextures(const char *data, char** res_data, size_t *res_len) {
  glip_debug("glip_impl_glGenTextures\n");
  GLsizei n =((GLsizei *)data)[0];
  *res_len = n*sizeof(GLuint);
  *res_data = (char *)malloc(*res_len);
  glad_glGenTextures(n, (GLuint*)*res_data);
}

void glip_impl_glTexImage2D(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glTexImage2D\n");
  GLsizei offset = 0;
  GLenum target = *((GLenum*)(data+offset));
  offset += sizeof(GLenum);
  GLint level = *((GLint*)(data+offset));
  offset += sizeof(GLint);
  GLint internalFormat = *((GLint*)(data+offset));
  offset += sizeof(GLint);
  GLsizei width = *((GLsizei*)(data+offset));
  offset += sizeof(GLsizei);
  GLsizei height = *((GLsizei*)(data+offset));
  offset += sizeof(GLsizei);
  GLint border = *((GLint*)(data+offset));
  offset += sizeof(GLint);
  GLenum format = *((GLenum*)(data+offset));
  offset += sizeof(GLenum);
  GLenum type = *((GLenum*)(data+offset));
  offset += sizeof(GLenum);
  /*GLsizei data_length = *((GLsizei*)(data+offset)); // data_length is unused*/
  offset += sizeof(GLsizei);
  glad_glTexImage2D(target, level, internalFormat, width, height, border, format, type, (const GLvoid *)data);
}

void glip_impl_glTexParameteri(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glTexParameteri\n");
  GLenum target =((GLenum *)data)[0];
  GLenum pname =((GLenum *)data)[1];
  GLint param =((GLint *)(data+2*sizeof(GLenum)))[0];
  glad_glTexParameteri(target, pname, param);
}

void glip_impl_glFramebufferTexture(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glFramebufferTexture\n");
  GLenum target =((GLenum *)data)[0];
  GLenum attachment =((GLenum *)data)[1];
  GLuint texture =((GLuint *)(data+2*sizeof(GLenum)))[0];
  GLint level =((GLint *)(data+2*sizeof(GLenum)+sizeof(GLuint)))[0];
  glad_glFramebufferTexture(target, attachment, texture, level);
}

void glip_impl_glBindTexture(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glBindTexture\n");
  GLenum target =((GLenum *)data)[0];
  GLuint texture =((GLuint *)(data+sizeof(GLenum)))[0];
  glad_glBindTexture(target, texture);
}

void glip_impl_glDrawBuffers(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glDrawBuffers\n");
  GLsizei n =((GLenum *)data)[0];
  data+=sizeof(GLsizei);
  glad_glDrawBuffers(n, (GLenum *)data);
}

void glip_impl_glGenBuffers(const char *data, char** res_data, size_t *res_len) {
  glip_debug("glip_impl_glGenBuffers\n");
  GLsizei n =((GLsizei *)data)[0];
  *res_len = n*sizeof(GLuint);
  *res_data = (char *)malloc(*res_len);
  glad_glGenBuffers(n, (GLuint*)*res_data);
}

void glip_impl_glBindBuffer(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glBindBuffer\n");
  GLenum target =((GLenum *)data)[0];
  GLuint buffer =((GLuint *)(data+sizeof(GLenum)))[0];
  glad_glBindBuffer(target, buffer);
}

void glip_impl_glBufferData(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glBufferData\n");
  GLenum target =((GLenum *)data)[0];
  GLsizeiptr size =((GLsizeiptr *)(data+sizeof(GLenum)))[0];
  GLenum usage =((GLenum *)(data+sizeof(GLenum)+sizeof(GLsizeiptr)+size))[0];
  glad_glBufferData(target, size, (const GLvoid *)(data+sizeof(GLenum)+sizeof(GLsizeiptr)), usage);
}

void glip_impl_glGenVertexArrays(const char *data, char** res_data, size_t *res_len) {
  glip_debug("glip_impl_glGenVertexArrays\n");
  GLsizei n =((GLsizei *)data)[0];
  *res_len = n*sizeof(GLuint);
  *res_data = (char *)malloc(*res_len);
  glad_glGenVertexArrays(n, (GLuint*)*res_data);
}

void glip_impl_glBindVertexArray(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glBindVertexArray\n");
  GLuint array =((GLuint *)(data))[0];
  glad_glBindVertexArray(array);
}

void glip_impl_glVertexAttribPointer(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glVertexAttribPointer\n");
  GLsizei offset = 0;
  GLuint index =((GLsizei *)(data+offset))[0];
  offset += sizeof(GLuint);
  GLint size =((GLsizei *)(data+offset))[0];
  offset += sizeof(GLint);
  GLenum type =((GLsizei *)(data+offset))[0];
  offset += sizeof(GLenum);
  GLboolean normalized =((GLsizei *)(data+offset))[0];
  offset += sizeof(GLboolean);
  GLsizei stride =((GLsizei *)(data+offset))[0];
  offset += sizeof(GLsizei);
  const GLvoid *pointer =((const GLvoid **)(data+offset))[0];
  
  glad_glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}

void glip_impl_glCheckFramebufferStatus(const char *data, char** res_data, size_t *res_len) {
  glip_debug("glip_impl_glCheckFramebufferStatus\n");
  *res_len = sizeof(GLenum);
  *res_data = (char *)malloc(*res_len);
  GLenum status = glad_glCheckFramebufferStatus(((GLenum *)data)[0]);
  memcpy(*res_data, &status, sizeof(GLuint));
}

void glip_impl_glGetAttribLocation(const char *data, char** res_data, size_t *res_len) {
  glip_debug("glip_impl_glGetAttribLocation\n");
  *res_len = sizeof(GLint);
  *res_data = (char *)malloc(*res_len);
  GLint location = glad_glGetAttribLocation(((GLenum *)data)[0], (data+sizeof(GLenum)));
  memcpy(*res_data, &location, sizeof(GLint));
}

void glip_impl_glEnableVertexAttribArray(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glEnableVertexAttribArray\n");
  glad_glEnableVertexAttribArray(((GLuint *)data)[0]);
}

void glip_impl_glEnable(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glEnable\n");
  glad_glEnable(((GLenum *)data)[0]);
}

void glip_impl_glClear(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glClear\n");
  glad_glClear(((GLbitfield *)data)[0]);
}

void glip_impl_glViewport(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glViewport\n");
  GLint x = ((GLint*)data)[0];
  GLint y = ((GLint*)data)[1];
  GLsizei width = ((GLint*)(data+2*sizeof(GLint)))[0];
  GLsizei height = ((GLint*)(data+2*sizeof(GLint)))[1];
  glad_glViewport(x, y, width, height);
}

void glip_impl_glGetUniformLocation(const char *data, char** res_data, size_t *res_len) {
  glip_debug("glip_impl_glGetUniformLocation\n");
  *res_len = sizeof(GLint);
  *res_data = (char *)malloc(*res_len);
  GLint location = glad_glGetUniformLocation(((GLenum *)data)[0], (data+sizeof(GLenum)));
  memcpy(*res_data, &location, sizeof(GLint));
}

void glip_impl_glUniform4fv(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glUniform4fv\n");
  GLint location = ((GLint *)data)[0];
  GLsizei count = ((GLsizei *)(data+sizeof(GLint)))[0];
  glad_glUniform4fv(location, count, (const GLfloat *)(data+sizeof(GLint)+sizeof(GLsizei)));
}

void glip_impl_glDrawArrays(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glDrawArrays\n");
  GLenum mode = ((GLenum *)data)[0];
  GLint first = ((GLint *)(data+sizeof(GLenum)))[0];
  GLsizei count = ((GLsizei *)(data+sizeof(GLenum)+sizeof(GLint)))[0];
  glip_debug("--> %d %d %d\n", mode, first, count);
  glad_glDrawArrays(mode, first, count);
}

void glip_impl_glGetError(const char *data, char** res_data, size_t *res_len) {
  (void) data;
  glip_debug("glip_impl_glGetError\n");
  GLenum err = glad_glGetError();
  *res_len = sizeof(GLenum);
  *res_data = (char *)malloc(*res_len);
  memcpy(*res_data, &err, *res_len);
}

void glip_impl_glGetString(const char *data, char** res_data, size_t *res_len) {
  glip_debug("glip_impl_glGetString\n");
  const GLubyte *str = glad_glGetString(*(GLenum *)data);
  *res_len = strlen((char *)str)+1;
  *res_data = (char *)malloc(*res_len);
  memcpy(*res_data, str, *res_len);
}

void glip_impl_glGetStringi(const char *data, char** res_data, size_t *res_len) {
  glip_debug("glip_impl_glGetStringi\n");
  GLenum name = *(GLenum *)data;
  GLuint index = *(GLuint *)(data+sizeof(GLenum));
  const GLubyte *str = glad_glGetStringi(name, index);
  *res_len = strlen((char *)str)+1;
  *res_data = (char *)malloc(*res_len);
  memcpy(*res_data, str, *res_len);
}

void glip_impl_glCullFace(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glCullFace\n");
  glip_debug("%d == %d\n", ((GLenum *)data)[0], 0x0405);
  glad_glCullFace(((GLenum *)data)[0]);
}

void glip_impl_glGenRenderbuffers(const char *data, char** res_data, size_t *res_len) {
  glip_debug("glip_impl_glGenRenderbuffers\n");
  GLsizei n =((GLsizei *)data)[0];
  *res_len = n*sizeof(GLuint);
  *res_data = (char *)malloc(*res_len);
  glad_glGenRenderbuffers(n, (GLuint*)*res_data);
}

void glip_impl_glBindRenderbuffer(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glBindRenderbuffer\n");
  GLenum target =((GLenum *)data)[0];
  GLuint renderbuffer =((GLuint *)(data+sizeof(GLenum)))[0];
  glad_glBindRenderbuffer(target, renderbuffer);
}

void glip_impl_glRenderbufferStorage(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glRenderbufferStorage\n");
  GLenum target =((GLenum *)data)[0];
  GLenum internalformat =((GLenum *)data)[1];
  GLsizei width =((GLsizei *)(data+2*sizeof(GLenum)))[0];
  GLsizei height =((GLsizei *)(data+2*sizeof(GLenum)))[1];
  glad_glRenderbufferStorage(target, internalformat, width, height);
}

void glip_impl_glFramebufferRenderbuffer(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glFramebufferRenderbuffer\n");
  GLenum target =((GLenum *)data)[0];
  GLenum attachment =((GLenum *)data)[1];
  GLenum renderbuffertarget =((GLenum *)data)[2];
  GLuint renderbuffer =((GLsizei *)(data+3*sizeof(GLenum)))[0];
  glad_glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
}

void glip_impl_glVertexAttribDivisor(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glVertexAttribDivisor\n");
  GLuint index =((GLuint *)data)[0];
  GLuint divisor =((GLuint *)data)[1];
  glad_glVertexAttribDivisor(index, divisor);
}

void glip_impl_glDrawElementsInstanced(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glDrawElementsInstanced\n");
  GLenum mode =((GLenum *)data)[0];
  GLsizei count =((GLsizei *)(data+sizeof(GLenum)))[0];
  GLenum type =((GLenum *)(data+sizeof(GLenum)+sizeof(GLsizei)))[0];
  GLsizei primcount =((GLsizei *)(data+2*sizeof(GLenum)+sizeof(GLsizei)))[0];
  GLboolean elem_buffer_bind =((GLboolean *)(data+2*sizeof(GLenum)+2*sizeof(GLsizei)))[0];
  void *indices = (void *)(data+2*sizeof(GLenum)+2*sizeof(GLsizei)+sizeof(GLboolean));
  if (elem_buffer_bind) {
    indices = *(void **)indices;
  }
  glad_glDrawElementsInstanced(mode, count, type, indices, primcount);
}

void glip_impl_glDrawArraysInstanced(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glDrawArraysInstanced\n");
  GLenum mode =((GLenum *)data)[0];
  GLint first =((GLint *)(data+sizeof(GLenum)))[0];
  GLsizei count =((GLsizei *)(data+sizeof(GLenum)+sizeof(GLint)))[0];
  GLsizei primcount =((GLsizei *)(data+sizeof(GLenum)+sizeof(GLint)))[1];
  glad_glDrawArraysInstanced(mode, first, count, primcount);
}

void glip_impl_glDisable(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glDisable\n");
  glad_glDisable(((GLenum *)data)[0]);
}

void glip_impl_glUniform1f(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glUniform1f\n");
  GLfloat v0 = ((GLfloat *)(data+sizeof(GLint)))[0];
  glad_glUniform1f(((GLint *)data)[0], v0);
}

void glip_impl_glUniform2f(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glUniform2f\n");
  GLfloat v0 = ((GLfloat *)(data+sizeof(GLint)))[0];
  GLfloat v1 = ((GLfloat *)(data+sizeof(GLint)))[1];
  glad_glUniform2f(((GLint *)data)[0], v0, v1);
}

void glip_impl_glUniform3f(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glUniform3f\n");
  GLfloat v0 = ((GLfloat *)(data+sizeof(GLint)))[0];
  GLfloat v1 = ((GLfloat *)(data+sizeof(GLint)))[1];
  GLfloat v2 = ((GLfloat *)(data+sizeof(GLint)))[2];
  glad_glUniform3f(((GLint *)data)[0], v0, v1, v2);
}

void glip_impl_glUniform4f(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glUniform4f\n");
  GLfloat v0 = ((GLfloat *)(data+sizeof(GLint)))[0];
  GLfloat v1 = ((GLfloat *)(data+sizeof(GLint)))[1];
  GLfloat v2 = ((GLfloat *)(data+sizeof(GLint)))[2];
  GLfloat v3 = ((GLfloat *)(data+sizeof(GLint)))[3];
  glad_glUniform4f(((GLint *)data)[0], v0, v1, v2, v3);
}

void glip_impl_glUniform1fv(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glUniform1fv\n");
  GLint location = ((GLint *)data)[0];
  GLsizei count = ((GLsizei *)(data+sizeof(GLint)))[0];
  glad_glUniform1fv(location, count, (const GLfloat *)(data+sizeof(GLint)+sizeof(GLsizei)));
}

void glip_impl_glUniform2fv(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glUniform2fv\n");
  GLint location = ((GLint *)data)[0];
  GLsizei count = ((GLsizei *)(data+sizeof(GLint)))[0];
  glad_glUniform2fv(location, count, (const GLfloat *)(data+sizeof(GLint)+sizeof(GLsizei)));
}

void glip_impl_glUniform3fv(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glUniform3fv\n");
  GLint location = ((GLint *)data)[0];
  GLsizei count = ((GLsizei *)(data+sizeof(GLint)))[0];
  glad_glUniform3fv(location, count, (const GLfloat *)(data+sizeof(GLint)+sizeof(GLsizei)));
}

void glip_impl_glUniformMatrix2fv(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glUniformMatrix2fv\n");
  GLint location = ((GLint *)data)[0];
  GLsizei count = ((GLsizei *)(data+sizeof(GLint)))[0];
  GLboolean transpose = ((GLboolean *)(data+sizeof(GLint)+sizeof(GLsizei)))[0];
  glad_glUniformMatrix2fv(location, count, transpose, (const GLfloat *)(data+sizeof(GLint)+sizeof(GLsizei)+sizeof(GLboolean)));
}

void glip_impl_glUniformMatrix3fv(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glUniformMatrix3fv\n");
  GLint location = ((GLint *)data)[0];
  GLsizei count = ((GLsizei *)(data+sizeof(GLint)))[0];
  GLboolean transpose = ((GLboolean *)(data+sizeof(GLint)+sizeof(GLsizei)))[0];
  glad_glUniformMatrix3fv(location, count, transpose, (const GLfloat *)(data+sizeof(GLint)+sizeof(GLsizei)+sizeof(GLboolean)));
}

void glip_impl_glUniformMatrix4fv(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glUniformMatrix4fv\n");
  GLint location = ((GLint *)data)[0];
  GLsizei count = ((GLsizei *)(data+sizeof(GLint)))[0];
  GLboolean transpose = ((GLboolean *)(data+sizeof(GLint)+sizeof(GLsizei)))[0];
  glad_glUniformMatrix4fv(location, count, transpose, (const GLfloat *)(data+sizeof(GLint)+sizeof(GLsizei)+sizeof(GLboolean)));
}

void glip_impl_glUniform1i(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glUniform1i\n");
  GLint v0 = ((GLint *)(data+sizeof(GLint)))[0];
  glad_glUniform1i(((GLint *)data)[0], v0);
}

void glip_impl_glUniform2i(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glUniform2i\n");
  GLint v0 = ((GLint *)(data+sizeof(GLint)))[0];
  GLint v1 = ((GLint *)(data+sizeof(GLint)))[1];
  glad_glUniform2i(((GLint *)data)[0], v0, v1);
}

void glip_impl_glUniform3i(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glUniform3i\n");
  GLint v0 = ((GLint *)(data+sizeof(GLint)))[0];
  GLint v1 = ((GLint *)(data+sizeof(GLint)))[1];
  GLint v2 = ((GLint *)(data+sizeof(GLint)))[2];
  glad_glUniform3i(((GLint *)data)[0], v0, v1, v2);
}

void glip_impl_glUniform4i(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glUniform4i\n");
  GLint v0 = ((GLint *)(data+sizeof(GLint)))[0];
  GLint v1 = ((GLint *)(data+sizeof(GLint)))[1];
  GLint v2 = ((GLint *)(data+sizeof(GLint)))[2];
  GLint v3 = ((GLint *)(data+sizeof(GLint)))[3];
  glad_glUniform4i(((GLint *)data)[0], v0, v1, v2, v3);
}

void glip_impl_glActiveTexture(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glActiveTexture\n");
  glad_glActiveTexture(*(GLenum *)data);
}

void glip_impl_glClearColor(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glClearColor\n");
  glad_glClearColor(((GLfloat *)data)[0], ((GLfloat *)data)[1], ((GLfloat *)data)[2], ((GLfloat *)data)[3]);
}

void glip_impl_glTexParameterf(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glTexParameterf\n");
  GLenum target =((GLenum *)data)[0];
  GLenum pname =((GLenum *)data)[1];
  GLfloat param =((GLfloat *)(data+2*sizeof(GLenum)))[0];
  glad_glTexParameterf(target, pname, param);
}

void glip_impl_glGenerateMipmap(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glGenerateMipmap\n");
  GLenum target =((GLenum *)data)[0];
  glad_glGenerateMipmap(target);
}

void glip_impl_glDeleteBuffers(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glDeleteBuffers\n");
  GLsizei n =((GLsizei *)data)[0];
  glad_glDeleteBuffers(n, (GLuint*)(data+sizeof(GLsizei)));
}

void glip_impl_glDeleteVertexArrays(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glDeleteVertexArrays\n");
  GLsizei n =((GLsizei *)data)[0];
  glad_glDeleteVertexArrays(n, (GLuint*)(data+sizeof(GLsizei)));
}

void glip_impl_glDeleteTextures(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glDeleteTextures\n");
  GLsizei n =((GLsizei *)data)[0];
  glad_glDeleteTextures(n, (GLuint*)(data+sizeof(GLsizei)));
}

void glip_impl_glDeleteFramebuffers(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glDeleteFramebuffers\n");
  GLsizei n =((GLsizei *)data)[0];
  glad_glDeleteFramebuffers(n, (GLuint*)(data+sizeof(GLsizei)));
}

void glip_impl_glReadBuffer(const char *data, char** res_data, size_t *res_len) {
  (void) res_data;
  (void) res_len;
  glip_debug("glip_impl_glReadBuffer\n");
  GLenum mode =((GLenum *)data)[0];
  glad_glReadBuffer(mode);
}

void glip_impl_glReadPixels(const char *data, char** res_data, size_t *res_len) {
  glip_debug("glip_impl_glReadPixels\n");
  GLint x, y;
  GLsizei width, height;
  GLenum format, type;
  x = ((GLint *)data)[0];
  y = ((GLint *)data)[1];
  width = ((GLint *)(data+2*sizeof(GLint)))[0];
  height = ((GLint *)(data+2*sizeof(GLint)))[1];
  format = ((GLenum *)(data+2*sizeof(GLint)+2*sizeof(GLsizei)))[0];
  type = ((GLenum *)(data+2*sizeof(GLint)+2*sizeof(GLsizei)))[1];
  switch (type) {
    case  GL_UNSIGNED_BYTE:
    case GL_BYTE:
    case GL_UNSIGNED_BYTE_3_3_2:
    case GL_UNSIGNED_BYTE_2_3_3_REV:
      *res_len = sizeof(GLbyte)*width*height;
      break;
    case GL_UNSIGNED_SHORT:
    case GL_SHORT:
    case GL_HALF_FLOAT:
    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_5_6_5_REV:
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_4_4_4_4_REV:
    case GL_UNSIGNED_SHORT_5_5_5_1:
    case GL_UNSIGNED_SHORT_1_5_5_5_REV:
      *res_len = sizeof(GLbyte)*width*height;
      break;
    case GL_UNSIGNED_INT:
    case GL_INT:
    case GL_FLOAT:
    case GL_UNSIGNED_INT_8_8_8_8:
    case GL_UNSIGNED_INT_8_8_8_8_REV:
    case GL_UNSIGNED_INT_10_10_10_2:
    case GL_UNSIGNED_INT_2_10_10_10_REV:
    case GL_UNSIGNED_INT_24_8:
    case GL_UNSIGNED_INT_10F_11F_11F_REV:
    case GL_UNSIGNED_INT_5_9_9_9_REV:
    case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
      *res_len = sizeof(GLbyte)*width*height;
      break;
    default:
      *res_len = 0;
      fprintf(stderr, "Invalid type %d in glip_impl_glReadPixels\n", type);
  }
  *res_data = (char *)malloc(*res_len);
  glad_glReadPixels(x, y, width, height, format, type, res_data);
}
