#include "egs.h"
#include "molecule_plugin.h"
#include <functional>
#include <vector>
#include <algorithm>

int main(void) {
  Context ctx;
  std::vector<std::shared_ptr<GLFWContext>> gl_ctx {
    std::make_shared<GLFWContext>(ctx, 200, 200, 200, 100),
    std::make_shared<GLFWContext>(ctx, 200, 200, 200, 300),
    std::make_shared<GLFWContext>(ctx, 200, 200, 200, 500),
    std::make_shared<GLFWContext>(ctx, 600, 600, 400, 100)
  };
  auto display_list = std::make_shared<DisplayList>();

  for (int i = 0; i<3; i++) {
    gl_ctx[i]->set_callback_function<Utils::CallbackTypes::MOUSEMOVE_CALLBACK>([] (GLContext &ctx, double x, double y, int state) {});
    gl_ctx[i]->set_callback_function<Utils::CallbackTypes::SCROLL_CALLBACK>([] (GLContext &ctx, double x, double y) {});
    gl_ctx[i]->set_callback_function<Utils::CallbackTypes::KEY_CALLBACK>([] (GLContext &ctx, int key, int mode) {});
  }

  display_list->add(molecule_plugin_create_molecule(ctx, "h2o.xyz"));

  while (std::any_of(gl_ctx.begin(), gl_ctx.end(), [] (std::shared_ptr<GLFWContext> ctx) {return ctx!=nullptr;})) {
    for (auto &ctx : gl_ctx) {
      if (ctx && !ctx->update(display_list)) {
        ctx = nullptr;
      }
    }
  }

  return 0;
}
