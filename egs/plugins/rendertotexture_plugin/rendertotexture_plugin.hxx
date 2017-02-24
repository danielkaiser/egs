#ifndef PLOT_PLUGIN_HXX
#define PLOT_PLUGIN_HXX

#include "rendertotexture_plugin.h"

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

namespace RenderToTexturePlugin {
  class Plot : public IDisplayListElement {
  public:
    Plot(std::shared_ptr<DisplayList> display_list, size_t width=500, size_t height=500, bool enable_mipmapping=true) : display_list(display_list), width(width), height(height), enable_mipmapping(enable_mipmapping) {}
    ~Plot();

    virtual void apply(GLContext& ctx);
    virtual void delete_handler(GLContext& ctx);

  private:
    std::shared_ptr<DisplayList> display_list;
    size_t width, height;
    bool enable_mipmapping;
    GLuint render_texture=0, framebuffer=0;
  };

  class Surface : public IDisplayListElement {
  public:
    Surface(glm::vec3 position, glm::vec3 width, glm::vec3 height);
    ~Surface();

    virtual void apply(GLContext& ctx);
    virtual void delete_handler(GLContext& ctx);

  private:
    glm::vec3 pos1, pos2, pos3, pos4;
    GLuint vao=0, vbo=0;
  };
};

#endif
