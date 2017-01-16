#ifndef CYLINDER_PLUGIN_HXX
#define CYLINDER_PLUGIN_HXX

#include "cylinder_plugin.h"

#include "displaylist.hxx"
#include "context.hxx"

#include <cstdint>
#include <cassert>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <iostream>
#include "../../glip/glip.h"
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

class Context;

namespace CylinderPlugin {
  class Cylinder : public IDisplayListElement {
  public:
    Cylinder(const std::vector<glm::vec3> &start, const std::vector<glm::vec3> &end, const std::vector<float> &radii, long color=0);
    ~Cylinder();
    virtual void apply(GLContext& ctx);

  private:
    GLuint vbo, instance_buffer, vao = 0;
    std::vector<glm::vec3> start_vec;
    std::vector<glm::vec3> end_vec;
    std::vector<float> radii_vec;
    long color;
  };
}


#endif
