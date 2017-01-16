#include "egs.h"
#include "molecule_plugin.h"

int main(void) {
  egs_context_ref ctx = egs_context_create();
  egs_gl_context_ref glfw_ctx = (egs_gl_context_ref)egs_glfw_context_create(ctx);
  egs_display_list_ref display_list = egs_display_list_create();

  egs_context_load_plugin(ctx, "molecule_plugin");
  egs_display_list_add_element(display_list, molecule_plugin_create_molecule(ctx, "h2o.xyz"));
  
  while (egs_gl_context_update(glfw_ctx, display_list));
  
  egs_display_list_destroy(display_list);
  egs_glip_glfw_context_destroy((egs_glip_glfw_context_ref)glfw_ctx);
  egs_context_destroy(ctx);
  return 0;
}
