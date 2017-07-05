//
//  function_pointer_dle.hpp
//  egs
//
//  Created by Daniel Kaiser on 13/05/16.
//  Copyright © 2016 Daniel Kaiser. All rights reserved.
//

#ifndef C_WRAPPER_DLE_H
#define C_WRAPPER_DLE_H

#include <iostream>
#include "context.hxx"
#include "propertystore.hxx"

#include "displaylist.hxx"

class CWrapper : public IDisplayListElement {
public:
  CWrapper(egs_cwrapper_t cwrapper): cwrapper(cwrapper) {}
  ~CWrapper();
 
  static void register_c_plugin(const char* plugin_name, egs_apply_fun apply, egs_delete_fun delete_fun, egs_terminate_fun terminate = nullptr);
  virtual void apply(GLContext& ctx);
  virtual void delete_handler(GLContext& ctx);

private:
  static std::unordered_map<std::string, egs_apply_fun> apply_ptr;
  static std::unordered_map<std::string, egs_delete_fun> delete_ptr;
  static std::unordered_map<std::string, egs_terminate_fun> terminate_ptr;
  egs_cwrapper_t cwrapper;
  c_property plugin_instance_data = nullptr;
  std::unordered_map<GLContext*, c_property> per_context_instance_data;
};

#endif /* function_pointer_dle_hpp */
