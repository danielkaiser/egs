#ifndef TRIANGLE_PLUGIN_HXX
#define TRIANGLE_PLUGIN_HXX

#include "triangle_plugin.h"

#include "egs.h"

#include <cstdint>
#include <cassert>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <iostream>
#include "glip.h"
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

class Context;

namespace TrianglePlugin {
  class Triangle : public IDisplayListElement {
  public:
    Triangle(const std::vector<glm::vec3> &vertices, long color=0) : vertices(vertices), normals({}), color(color) {assert((vertices.size()%3) == 0);}
    Triangle(const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &normals, long color=0) : vertices(vertices), normals(normals), color(color)
    {assert((vertices.size()%3) == 0); assert(vertices.size() == normals.size());}
    ~Triangle();

    virtual void apply(GLContext& ctx);
  private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    long color;
    GLuint vao=0, vbo=0;
  };
};

#endif
