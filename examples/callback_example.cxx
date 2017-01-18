#include "egs.h"
#include "callback_plugin.hxx"

int main(void) {
  Context ctx;
  GLFWContext glfw_ctx = GLFWContext(ctx);
  auto display_list = std::make_shared<DisplayList>();

  const char *text = "Hello EGS!";
  display_list->add(std::make_shared<CallbackPlugin::Callback>([&text] (GLContext& gl_ctx) {
    (void)gl_ctx;
    std::cout << text << std::endl;
  }));

  while (glfw_ctx.update(display_list));

  return 0;
}
