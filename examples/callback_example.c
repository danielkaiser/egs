#include "egs.h"
#include "callback_plugin.h"

void callback(egs_gl_context_ref gl_ctx_ref, void *user_data) {
  (void)gl_ctx_ref;
  printf("%s\n", (const char *)user_data);
}

int main(void) {
  egs_context_ref ctx = egs_context_create();
  egs_gl_context_ref glfw_ctx = (egs_gl_context_ref)egs_glfw_context_create(ctx);
  egs_display_list_ref display_list = egs_display_list_create();

  egs_context_load_plugin(ctx, "callback_plugin");
  egs_display_list_add_element(display_list, callback_plugin_create_callback(ctx, callback, "Hello EGS!"));

  while (egs_gl_context_update(glfw_ctx, display_list));

  egs_display_list_destroy(display_list);
  egs_glip_glfw_context_destroy((egs_glip_glfw_context_ref)glfw_ctx);
  egs_context_destroy(ctx);
  return 0;
}
