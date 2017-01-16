/*
//  glip_impl.h
//  egs
//
//  Created by Daniel Kaiser on 01/07/16.
//  Copyright © 2016 Daniel Kaiser. All rights reserved.
*/

#ifndef glip_impl_h
#define glip_impl_h

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void glip_impl_glCreateShader(const char *data, char** res_data, size_t *res_len);
void glip_impl_glShaderSource(const char *data, char** res_data, size_t *res_len);
void glip_impl_glCompileShader(const char *data, char** res_data, size_t *res_len);
void glip_impl_glGetShaderiv(const char *data, char** res_data, size_t *res_len);
void glip_impl_glGetShaderInfoLog(const char *data, char** res_data, size_t *res_len);
void glip_impl_glDeleteShader(const char *data, char** res_data, size_t *res_len);
void glip_impl_glCreateProgram(const char *data, char** res_data, size_t *res_len);
void glip_impl_glAttachShader(const char *data, char** res_data, size_t *res_len);
void glip_impl_glBindFragDataLocation(const char *data, char** res_data, size_t *res_len);
void glip_impl_glLinkProgram(const char *data, char** res_data, size_t *res_len);
void glip_impl_glGetProgramiv(const char *data, char** res_data, size_t *res_len);
void glip_impl_glGetProgramInfoLog(const char *data, char** res_data, size_t *res_len);
void glip_impl_glDeleteProgram(const char *data, char** res_data, size_t *res_len);
void glip_impl_glUseProgram(const char *data, char** res_data, size_t *res_len);
void glip_impl_glGetIntegerv(const char *data, char** res_data, size_t *res_len);
void glip_impl_glGenFramebuffers(const char *data, char** res_data, size_t *res_len);
void glip_impl_glBindFramebuffer(const char *data, char** res_data, size_t *res_len);
void glip_impl_glGenTextures(const char *data, char** res_data, size_t *res_len);
void glip_impl_glTexImage2D(const char *data, char** res_data, size_t *res_len);
void glip_impl_glTexParameteri(const char *data, char** res_data, size_t *res_len);
void glip_impl_glFramebufferTexture(const char *data, char** res_data, size_t *res_len);
void glip_impl_glBindTexture(const char *data, char** res_data, size_t *res_len);
void glip_impl_glDrawBuffers(const char *data, char** res_data, size_t *res_len);
void glip_impl_glGenBuffers(const char *data, char** res_data, size_t *res_len);
void glip_impl_glBindBuffer(const char *data, char** res_data, size_t *res_len);
void glip_impl_glBufferData(const char *data, char** res_data, size_t *res_len);
void glip_impl_glGenVertexArrays(const char *data, char** res_data, size_t *res_len);
void glip_impl_glBindVertexArray(const char *data, char** res_data, size_t *res_len);
void glip_impl_glVertexAttribPointer(const char *data, char** res_data, size_t *res_len);
void glip_impl_glCheckFramebufferStatus(const char *data, char** res_data, size_t *res_len);
void glip_impl_glGetAttribLocation(const char *data, char** res_data, size_t *res_len);
void glip_impl_glEnableVertexAttribArray(const char *data, char** res_data, size_t *res_len);
void glip_impl_glEnable(const char *data, char** res_data, size_t *res_len);
void glip_impl_glClear(const char *data, char** res_data, size_t *res_len);
void glip_impl_glViewport(const char *data, char** res_data, size_t *res_len);
void glip_impl_glGetUniformLocation(const char *data, char** res_data, size_t *res_len);
void glip_impl_glUniform4fv(const char *data, char** res_data, size_t *res_len);
void glip_impl_glDrawArrays(const char *data, char** res_data, size_t *res_len);
void glip_impl_glGetError(const char *data, char** res_data, size_t *res_len);
void glip_impl_glGetString(const char *data, char** res_data, size_t *res_len);
void glip_impl_glGetStringi(const char *data, char** res_data, size_t *res_len);
void glip_impl_glCullFace(const char *data, char** res_data, size_t *res_len);
void glip_impl_glGenRenderbuffers(const char *data, char** res_data, size_t *res_len);
void glip_impl_glBindRenderbuffer(const char *data, char** res_data, size_t *res_len);
void glip_impl_glRenderbufferStorage(const char *data, char** res_data, size_t *res_len);
void glip_impl_glFramebufferRenderbuffer(const char *data, char** res_data, size_t *res_len);
void glip_impl_glVertexAttribDivisor(const char *data, char** res_data, size_t *res_len);
void glip_impl_glDrawElementsInstanced(const char *data, char** res_data, size_t *res_len);
void glip_impl_glDrawArraysInstanced(const char *data, char** res_data, size_t *res_len);
void glip_impl_glDisable(const char *data, char** res_data, size_t *res_len);
void glip_impl_glUniform1f(const char *data, char** res_data, size_t *res_len);
void glip_impl_glUniform2f(const char *data, char** res_data, size_t *res_len);
void glip_impl_glUniform3f(const char *data, char** res_data, size_t *res_len);
void glip_impl_glUniform4f(const char *data, char** res_data, size_t *res_len);
void glip_impl_glUniform1fv(const char *data, char** res_data, size_t *res_len);
void glip_impl_glUniform2fv(const char *data, char** res_data, size_t *res_len);
void glip_impl_glUniform3fv(const char *data, char** res_data, size_t *res_len);
void glip_impl_glUniformMatrix2fv(const char *data, char** res_data, size_t *res_len);
void glip_impl_glUniformMatrix3fv(const char *data, char** res_data, size_t *res_len);
void glip_impl_glUniformMatrix4fv(const char *data, char** res_data, size_t *res_len);
void glip_impl_glUniform1i(const char *data, char** res_data, size_t *res_len);
void glip_impl_glUniform2i(const char *data, char** res_data, size_t *res_len);
void glip_impl_glUniform3i(const char *data, char** res_data, size_t *res_len);
void glip_impl_glUniform4i(const char *data, char** res_data, size_t *res_len);
void glip_impl_glActiveTexture(const char *data, char** res_data, size_t *res_len);
void glip_impl_glClearColor(const char *data, char** res_data, size_t *res_len);
void glip_impl_glTexParameterf(const char *data, char** res_data, size_t *res_len);
void glip_impl_glGenerateMipmap(const char *data, char** res_data, size_t *res_len);
void glip_impl_glDeleteBuffers(const char *data, char** res_data, size_t *res_len);
void glip_impl_glDeleteVertexArrays(const char *data, char** res_data, size_t *res_len);
void glip_impl_glDeleteTextures(const char *data, char** res_data, size_t *res_len);
void glip_impl_glDeleteFramebuffers(const char *data, char** res_data, size_t *res_len);
void glip_impl_glReadBuffer(const char *data, char** res_data, size_t *res_len);
void glip_impl_glReadPixels(const char *data, char** res_data, size_t *res_len);
  
void (*const glip_implementations[])(const char *, char**, size_t *) = {
  glip_impl_glCreateShader,
  glip_impl_glShaderSource,
  glip_impl_glCompileShader,
  glip_impl_glGetShaderiv,
  glip_impl_glGetShaderInfoLog,
  glip_impl_glDeleteShader,
  glip_impl_glCreateProgram,
  glip_impl_glAttachShader,
  glip_impl_glBindFragDataLocation,
  glip_impl_glLinkProgram,
  glip_impl_glGetProgramiv,
  glip_impl_glGetProgramInfoLog,
  glip_impl_glDeleteProgram,
  glip_impl_glUseProgram,
  glip_impl_glGetIntegerv,
  glip_impl_glGenFramebuffers,
  glip_impl_glBindFramebuffer,
  glip_impl_glGenTextures,
  glip_impl_glTexImage2D,
  glip_impl_glTexParameteri,
  glip_impl_glFramebufferTexture,
  glip_impl_glBindTexture,
  glip_impl_glDrawBuffers,
  glip_impl_glGenBuffers,
  glip_impl_glBindBuffer,
  glip_impl_glBufferData,
  glip_impl_glGenVertexArrays,
  glip_impl_glBindVertexArray,
  glip_impl_glVertexAttribPointer,
  glip_impl_glCheckFramebufferStatus,
  glip_impl_glGetAttribLocation,
  glip_impl_glEnableVertexAttribArray,
  glip_impl_glEnable,
  glip_impl_glClear,
  glip_impl_glViewport,
  glip_impl_glGetUniformLocation,
  glip_impl_glUniform4fv,
  glip_impl_glDrawArrays,
  glip_impl_glGetError,
  glip_impl_glGetString,
  glip_impl_glGetStringi,
  glip_impl_glCullFace,
  glip_impl_glGenRenderbuffers,
  glip_impl_glBindRenderbuffer,
  glip_impl_glRenderbufferStorage,
  glip_impl_glFramebufferRenderbuffer,
  glip_impl_glVertexAttribDivisor,
  glip_impl_glDrawElementsInstanced,
  glip_impl_glDrawArraysInstanced,
  glip_impl_glDisable,
  glip_impl_glUniform1f,
  glip_impl_glUniform2f,
  glip_impl_glUniform3f,
  glip_impl_glUniform4f,
  glip_impl_glUniform1fv,
  glip_impl_glUniform2fv,
  glip_impl_glUniform3fv,
  glip_impl_glUniformMatrix2fv,
  glip_impl_glUniformMatrix3fv,
  glip_impl_glUniformMatrix4fv,
  glip_impl_glUniform1i,
  glip_impl_glUniform2i,
  glip_impl_glUniform3i,
  glip_impl_glUniform4i,
  glip_impl_glActiveTexture,
  glip_impl_glClearColor,
  glip_impl_glTexParameterf,
  glip_impl_glGenerateMipmap,
  glip_impl_glDeleteBuffers,
  glip_impl_glDeleteVertexArrays,
  glip_impl_glDeleteTextures,
  glip_impl_glDeleteFramebuffers,
  glip_impl_glReadBuffer,
  glip_impl_glReadPixels,
};
  
#ifdef __cplusplus
}
#endif

#endif /* glip_impl_h */
