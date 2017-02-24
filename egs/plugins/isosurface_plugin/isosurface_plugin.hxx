#ifndef ISOSURFACE_PLUGIN_HXX
#define ISOSURFACE_PLUGIN_HXX

#include "isosurface_plugin.h"
#include "../triangle_plugin/triangle_plugin.hxx"

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

namespace IsosurfacePlugin {
  class Isosurface : public IDisplayListElement {
  public:
    Isosurface(const std::vector<float> &x_values, glm::vec2 x_range,
               const std::vector<float> &y_values, glm::vec2 y_range,
               const std::vector<float> &z_values, glm::vec2 z_range) : x_values(x_values), y_values(y_values), z_values(z_values),
                                                                          x_range(x_range), y_range(y_range), z_range(z_range){}
    ~Isosurface();
    virtual void apply(GLContext& ctx);
    virtual void delete_handler(GLContext& ctx);

  private:
    std::vector<float> x_values;
    std::vector<float> y_values;
    std::vector<float> z_values;
    glm::vec2 x_range;
    glm::vec2 y_range;
    glm::vec2 z_range;
    std::vector<TrianglePlugin::Triangle> triangles;
  };
};

#endif
