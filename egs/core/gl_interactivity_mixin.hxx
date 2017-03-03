#ifndef GL_INTERACTIVITY_MIXIN_H
#define GL_INTERACTIVITY_MIXIN_H

#include <functional>
#include <iostream>
#include "gl_context.hxx"
#include "gl_interactivity_keycodes.hxx"

namespace Utils {
  enum class CallbackTypes {
    CHAR_CALLBACK,
    KEY_CALLBACK,
    MOUSEMOVE_CALLBACK,
    SCROLL_CALLBACK,
    RESIZE_CALLBACK
  };

  template <CallbackTypes c>
  struct defaultCallbackImpl;

  template<>
  struct defaultCallbackImpl<CallbackTypes::CHAR_CALLBACK> {
    static void callback(GLContext &ctx, unsigned int character);
  };

  template<>
  struct defaultCallbackImpl<CallbackTypes::KEY_CALLBACK> {
    static void callback(GLContext &ctx, int key, int action);
  };

  template<>
  struct defaultCallbackImpl<CallbackTypes::MOUSEMOVE_CALLBACK> {
    static void callback(GLContext &ctx, double xpos, double ypos, int mouse_state);
  };

  template<>
  struct defaultCallbackImpl<CallbackTypes::SCROLL_CALLBACK> {
    static void callback(GLContext &ctx, double xoffset, double yoffset);
  };

  template<>
  struct defaultCallbackImpl<CallbackTypes::RESIZE_CALLBACK> {
    static void callback(GLContext &ctx, int width, int height);
  };
}

class GLInteractivityMixin {
public:
  GLInteractivityMixin(GLContext &ctx) : ctx(ctx) {}
  using CallbackTypes = Utils::CallbackTypes;

  template <CallbackTypes c>
  using defaultCallbacks = Utils::defaultCallbackImpl<c>;

  template <CallbackTypes c>
  void set_callback_function(const std::function<decltype(defaultCallbacks<c>::callback)> &cb_func);

protected:
  std::function<decltype(defaultCallbacks<CallbackTypes::CHAR_CALLBACK>::callback)> char_callback = defaultCallbacks<CallbackTypes::CHAR_CALLBACK>::callback;
  std::function<decltype(defaultCallbacks<CallbackTypes::KEY_CALLBACK>::callback)> key_callback = defaultCallbacks<CallbackTypes::KEY_CALLBACK>::callback;
  std::function<decltype(defaultCallbacks<CallbackTypes::MOUSEMOVE_CALLBACK>::callback)> mouse_callback = defaultCallbacks<CallbackTypes::MOUSEMOVE_CALLBACK>::callback;
  std::function<decltype(defaultCallbacks<CallbackTypes::SCROLL_CALLBACK>::callback)> scroll_callback = defaultCallbacks<CallbackTypes::SCROLL_CALLBACK>::callback;
  std::function<decltype(defaultCallbacks<CallbackTypes::RESIZE_CALLBACK>::callback)> resize_callback = defaultCallbacks<CallbackTypes::RESIZE_CALLBACK>::callback;
private:
  GLContext &ctx;
};

#endif
