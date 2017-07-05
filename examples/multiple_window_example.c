#include "egs.h"
#include "molecule_plugin.h"


int main(void) {
  int ctx_cnt = 4;
  egs_context_ref ctx = egs_context_create();
  egs_glip_glfw_context_ref glfw_ctx[4] = {
      (egs_glip_glfw_context_ref) egs_glip_glfw_context_create(ctx, 200, 200, 200, 100),
      (egs_glip_glfw_context_ref) egs_glip_glfw_context_create(ctx, 200, 200, 200, 300),
      (egs_glip_glfw_context_ref) egs_glip_glfw_context_create(ctx, 200, 200, 200, 500),
      (egs_glip_glfw_context_ref) egs_glip_glfw_context_create(ctx, 600, 600, 400, 100)
  };

  egs_display_list_ref display_list = egs_display_list_create();

  egs_camera_movement_look_at((egs_camera_movement_ref) ctx, 0, 0, 3, 0, 0, 0, 0, 1, 0);
  egs_camera_movement_look_at((egs_camera_movement_ref) glfw_ctx[1], 0, -3, 0, 0, 0, 0, 0, 0, 1);
  egs_camera_movement_look_at((egs_camera_movement_ref) glfw_ctx[2], -3, 0, 0, 0, 0, 0, 0, 1, 0);

  egs_context_load_plugin(ctx, "molecule_plugin");
  egs_display_list_add_element(display_list, molecule_plugin_create_molecule(ctx, "h2o.xyz"));
  
  while (ctx_cnt) {
    for (int i = 0; i < 4; i++) {
      if (glfw_ctx[i]) {
        if (!egs_gl_context_update((egs_gl_context_ref) glfw_ctx[i], display_list)) {
          egs_glip_glfw_context_destroy((egs_glip_glfw_context_ref)glfw_ctx[i]);
          glfw_ctx[i] = NULL;
          ctx_cnt--;
        }
      }
    }
  }
  
  egs_display_list_destroy(display_list);
  egs_context_destroy(ctx);
  return 0;
}
