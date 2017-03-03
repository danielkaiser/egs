#include "gl_interactivity_mixin.hxx"

void Utils::defaultCallbackImpl<Utils::CallbackTypes::CHAR_CALLBACK>::callback(GLContext &ctx, unsigned int character) {
}

void Utils::defaultCallbackImpl<Utils::CallbackTypes::KEY_CALLBACK>::callback(GLContext &ctx, int key, int action) {
  if (key == EGS_KEY_LEFT && (action == EGS_PRESS || action == EGS_REPEAT)) {
    ctx.rotate(glm::vec3(0,1,0), 0.1);
  } else if (key == EGS_KEY_RIGHT && (action == EGS_PRESS || action == EGS_REPEAT)) {
    ctx.rotate(glm::vec3(0,1,0), -0.1);
  } else if (key == EGS_KEY_UP && (action == EGS_PRESS || action == EGS_REPEAT)) {
    ctx.rotate(glm::vec3(1,0,0), 0.1);
  } else if (key == EGS_KEY_DOWN && (action == EGS_PRESS || action == EGS_REPEAT)) {
    ctx.rotate(glm::vec3(1,0,0), -0.1);
  }

  if (key == EGS_KEY_W && (action == EGS_PRESS || action == EGS_REPEAT)) {
    ctx.translate(glm::vec3(0,1,0));
  } else if (key == EGS_KEY_S && (action == EGS_PRESS || action == EGS_REPEAT)) {
    ctx.translate(glm::vec3(0, -1,0));
  } else if (key == EGS_KEY_A && (action == EGS_PRESS || action == EGS_REPEAT)) {
    ctx.translate(glm::vec3(-1,0,0));
  } else if (key == EGS_KEY_D && (action == EGS_PRESS || action == EGS_REPEAT)) {
    ctx.translate(glm::vec3(1,0,0));
  } else if (key == EGS_KEY_T && (action == EGS_PRESS || action == EGS_REPEAT)) {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tstruct);
    ctx.draw_png("egs_"+std::string(buf)+".png", 4096, 4096);
  }
}

void Utils::defaultCallbackImpl<Utils::CallbackTypes::MOUSEMOVE_CALLBACK>::callback(GLContext &ctx, double xpos, double ypos, int mouse_state) {
  static glm::vec3 pos_old = {0, 0, 0};
  glm::vec3 pos_new(xpos/ctx.get_width(), ypos/-ctx.get_height(), 0);
  if (mouse_state && pos_old != pos_new) {
    glm::vec3 d(pos_new.y - pos_old.y, - pos_new.x + pos_old.x, 0);
    double len = glm::length(d);
    if (len > 0) {
      ctx.rotate(glm::normalize(d), len);
    }
  }
  pos_old = pos_new;
}

void Utils::defaultCallbackImpl<Utils::CallbackTypes::SCROLL_CALLBACK>::callback(GLContext &ctx, double xoffset, double yoffset) {
  ctx.zoom(1-0.1*fmax(fmin(yoffset, 1), -1));
}

void Utils::defaultCallbackImpl<Utils::CallbackTypes::RESIZE_CALLBACK>::callback(GLContext &ctx, int width, int height) {
  glViewport(0, 0, width, height);
  ctx.set_perspective(45, 1.0*width/height);
}

template<>
void GLInteractivityMixin::set_callback_function<Utils::CallbackTypes::CHAR_CALLBACK>(const std::function<decltype(defaultCallbacks<Utils::CallbackTypes::CHAR_CALLBACK>::callback)> &cb_func) {
  char_callback = cb_func;
}

template<>
void GLInteractivityMixin::set_callback_function<Utils::CallbackTypes::KEY_CALLBACK>(const std::function<decltype(defaultCallbacks<Utils::CallbackTypes::KEY_CALLBACK>::callback)> &cb_func) {
  key_callback = cb_func;
}

template<>
void GLInteractivityMixin::set_callback_function<Utils::CallbackTypes::MOUSEMOVE_CALLBACK>(const std::function<decltype(defaultCallbacks<Utils::CallbackTypes::MOUSEMOVE_CALLBACK>::callback)> &cb_func) {
  mouse_callback = cb_func;
}

template<>
void GLInteractivityMixin::set_callback_function<Utils::CallbackTypes::SCROLL_CALLBACK>(const std::function<decltype(defaultCallbacks<Utils::CallbackTypes::SCROLL_CALLBACK>::callback)> &cb_func) {
  scroll_callback = cb_func;
}

template<>
void GLInteractivityMixin::set_callback_function<Utils::CallbackTypes::RESIZE_CALLBACK>(const std::function<decltype(defaultCallbacks<Utils::CallbackTypes::RESIZE_CALLBACK>::callback)> &cb_func) {
  resize_callback = cb_func;
}
