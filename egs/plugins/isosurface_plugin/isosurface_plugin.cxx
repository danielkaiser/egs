#include "isosurface_plugin.hxx"
#include <glm/vec4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/transform.hpp>

void isosurface_plugin_init_plugin(egs_context_ref ctx_ref) {
  (void) ctx_ref;
}

void isosurface_plugin_terminate_plugin() {

}

IsosurfacePlugin::Isosurface::~Isosurface() {
  egs_printf(EGS_DEBUG, "isosurface destructor\n");
  for (auto ctx : registered_gl_contexts) {
    delete_handler(*ctx);
  }
}

void IsosurfacePlugin::Isosurface::apply(GLContext& ctx) {
  if (triangles.size() == 0) {
    std::vector<glm::vec3> vertices;
    egs_printf(EGS_DEBUG, "isosurface dims: %d %d\n", x_values.size(), y_values.size());
    for (size_t i=0; i<x_values.size()-1; i++) {
      for (size_t j=0; j<y_values.size()-1; j++) {
        vertices.push_back(glm::vec3(x_values[i], z_values[j*x_values.size()+i], y_values[j]));

        vertices.push_back(glm::vec3(x_values[i], z_values[(j+1)*x_values.size()+i], y_values[j+1]));
        vertices.push_back(glm::vec3(x_values[i+1], z_values[j*x_values.size()+i+1], y_values[j]));

        vertices.push_back(glm::vec3(x_values[i+1], z_values[j*x_values.size()+i+1], y_values[j]));
        vertices.push_back(glm::vec3(x_values[i], z_values[(j+1)*x_values.size()+i], y_values[j+1]));
        vertices.push_back(glm::vec3(x_values[i+1], z_values[(j+1)*x_values.size()+i+1], y_values[j+1]));
      }
    }
    triangles.push_back(TrianglePlugin::Triangle(vertices, 0x00b2ff));
    egs_printf(EGS_DEBUG, "isosurface created\n");
  }
  for (auto it = triangles.begin(); it < triangles.end(); it++) {
    it->apply(ctx);
  }
}

void IsosurfacePlugin::Isosurface::delete_handler(GLContext& ctx) {
  egs_printf(EGS_DEBUG, "isosurface delete handler called\n");
}

/*display_list_elem_ref triangle_plugin_create_triangle(int n_points, float *vertices, float* normals, long color=0) {
  assert((n_points%3) == 0);
  std::vector<glm::vec3> vert;
  for (int i=0; i<n_points; i++) {
    vert.push_back(glm::vec3(vertices[3*i], vertices[3*i+1], vertices[3*i+2]));
  }
  if (normals) {
    std::vector<glm::vec3> norm;
    for (int i=0; i<n_points; i++) {
      norm.push_back(glm::vec3(normals[3*i], normals[3*i+1], normals[3*i+2]));
    }
    return reinterpret_cast<display_list_elem_ref>(new TrianglePlugin::Triangle(vert, norm, color));
  } else {
    return reinterpret_cast<display_list_elem_ref>(new TrianglePlugin::Triangle(vert, color));
  }
}
*/
