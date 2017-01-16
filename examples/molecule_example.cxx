#include "egs.h"
#include "molecule_plugin.h"

int main(void) {
  Context ctx;
  GLFWContext glfw_ctx = GLFWContext(ctx);
  auto display_list = std::make_shared<DisplayList>();

  display_list->add(molecule_plugin_create_molecule(ctx, "h2o.xyz"));

  while (glfw_ctx.update(display_list));

  return 0;
}
