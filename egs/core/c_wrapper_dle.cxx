//
//  function_pointer_dle.cpp
//  egs
//
//  Created by Daniel Kaiser on 13/05/16.
//  Copyright © 2016 Daniel Kaiser. All rights reserved.
//

#include "c_wrapper_dle.hxx"

std::unordered_map<std::string, egs_apply_fun> CWrapper::apply_ptr;
std::unordered_map<std::string, egs_terminate_fun> CWrapper::terminate_ptr;

CWrapper::~CWrapper() {
  if ((&cwrapper)==0 || !cwrapper.plugin_name) {
    egs_printf(EGS_WARNING, "Could not deinitialize plugin instance.\n");
  }
  else if (terminate_ptr.find(cwrapper.plugin_name) == terminate_ptr.end() || terminate_ptr[cwrapper.plugin_name] == nullptr) {
    egs_printf(EGS_WARNING, "Could not deinitialize %s plugin instance.\n", cwrapper.plugin_name);
  } else {
    egs_printf(EGS_DEBUG, "terminate function for %s instance:  %p\n", cwrapper.plugin_name, terminate_ptr[cwrapper.plugin_name]);
    terminate_ptr[cwrapper.plugin_name](cwrapper.data_length, cwrapper.data, &plugin_instance_data);
  }
}

void egs_c_wrapper_register_c_plugin(const char* plugin_name, egs_apply_fun apply, egs_terminate_fun terminate) {
  CWrapper::register_c_plugin(plugin_name, apply, terminate);
}

egs_display_list_elem_ref egs_c_wrapper_create(egs_cwrapper_t wrapper) {
  CWrapper *cwrap = new CWrapper(wrapper);
  return reinterpret_cast<egs_display_list_elem_ref>(cwrap);
}

void CWrapper::register_c_plugin(const char* plugin_name, egs_apply_fun apply, egs_terminate_fun terminate) {
  apply_ptr[plugin_name] = apply;
  terminate_ptr[plugin_name] = terminate;
}

void CWrapper::apply(GLContext& ctx) {
  egs_printf(EGS_DEBUG, "applying cwrapper for %s\n", cwrapper.plugin_name);
  if (apply_ptr.find(cwrapper.plugin_name) == apply_ptr.end()) {
    egs_printf(EGS_WARNING, "EGS: warning: C-plugin %s not found in apply method!\n", cwrapper.plugin_name);
    if (!ctx.get_context().load_plugin(cwrapper.plugin_name)) {
      egs_printf(EGS_WARNING, "EGS: warning: C-plugin %s could not be loaded, skipping!\n", cwrapper.plugin_name);
      return;
    }
  }
  assert(apply_ptr[cwrapper.plugin_name]);
  apply_ptr[cwrapper.plugin_name](reinterpret_cast<egs_gl_context_ref>(&ctx), cwrapper.data_length, cwrapper.data, &plugin_instance_data);
}
