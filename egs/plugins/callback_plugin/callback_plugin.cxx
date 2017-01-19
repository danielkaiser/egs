#include "callback_plugin.hxx"

void callback_plugin_init_plugin(egs_context_ref ctx_ref) {
  (void)ctx_ref;
}

void callback_plugin_terminate_plugin() {
}

CallbackPlugin::Callback::Callback(const callback_type& callback) : m_callback(callback) {
}

CallbackPlugin::Callback::~Callback() {
}

void CallbackPlugin::Callback::apply(GLContext& ctx) {
  (void)ctx;
  m_callback(ctx);
}

egs_display_list_elem_ref callback_plugin_create_callback(egs_context_ref ctx, void (*callback)(egs_gl_context_ref ctx, void *user_data), void *user_data) {
  (void) ctx;
  CallbackPlugin::Callback::callback_type cxx_callback = [=] (GLContext& ctx) {
    egs_gl_context_ref gl_ctx_ref = reinterpret_cast<egs_gl_context_ref>(&ctx);
    callback(gl_ctx_ref, user_data);
  };
  return reinterpret_cast<egs_display_list_elem_ref>(new CallbackPlugin::Callback(cxx_callback));
}
