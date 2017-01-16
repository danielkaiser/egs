#include "compile_shader.hxx"
#include "c_api.h"
#include "context.hxx"

#include <cassert>
#include <vector>
#include <fstream>
#include <iostream>

GLuint Util::compile_shader_file(const GLuint shader_type, const std::string file_name) {
  std::string shader_path = Context::get_plugin_path();
  std::ifstream file(shader_path + file_name);
  std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  return compile_shader(shader_type, str);
}

GLuint Util::compile_shader(const GLuint shader_type, const std::string shader_src) {
  int compiled;
  assert(glCreateShader);
  GLuint shader = glCreateShader(shader_type);
  const char *shader_src_ptr = shader_src.c_str();
  glShaderSource(shader, 1, &shader_src_ptr, nullptr);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    GLint maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
    for (auto c: errorLog) {
      std::cerr << c;
    }
    std::cerr << std::endl;
    glDeleteShader(shader);
  }
  return shader;
}

GLuint Util::link_shader_program(const std::vector<GLuint> &shader, const std::string &frag_data_location) {
  int compiled;
  GLuint program = glCreateProgram();
  for (size_t i=0; i<shader.size(); i++) {
    glAttachShader(program, shader[i]);
  }
  glBindFragDataLocation(program, 0, frag_data_location.c_str());
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &compiled);
  if (!compiled) {
    GLint maxLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
    std::vector<GLchar> infoLog(maxLength);
    glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
    for (auto c: infoLog) {
      std::cerr << c;
    }
    std::cerr << std::endl;
    glDeleteProgram(program);
  }
  for (auto s: shader) {
    glDeleteShader(s);
  }
  return program;
}

GLuint egs_util_compile_shader(const GLuint shader_type, const char *shader_src) {
  return Util::compile_shader(shader_type, shader_src);
}

GLuint egs_util_compile_shader_file(const GLuint shader_type, const char *file_name) {
  return Util::compile_shader_file(shader_type, file_name);
}
GLuint egs_util_link_shader_program(const int n, const GLuint *shader, const char *frag_data_location) {
  std::vector<GLuint> shader_vec(n);
  for (int i = 0; i < n; i++) {
    shader_vec[i] = shader[i];
  }
  return Util::link_shader_program(shader_vec, frag_data_location);
}
