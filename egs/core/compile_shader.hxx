#ifndef COMPILE_SHADER_H
#define COMPILE_SHADER_H

#include <string>
#include <vector>
#include "glip.h"
#include <GLFW/glfw3.h>

namespace Util {
  GLuint compile_shader(const GLuint shader_type, const std::string shader_src);
  GLuint compile_shader_file(const GLuint shader_type, const std::string file_name);
  GLuint link_shader_program(const std::vector<GLuint> &shader, const std::string &frag_data_location);
}

#endif /* compile_shader_hpp */
