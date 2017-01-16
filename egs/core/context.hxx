#ifndef CONTEXT_HXX
#define CONTEXT_HXX

#include "propertystore.hxx"
#include "displaylist.hxx"
#include <memory>
#include <unordered_map>
#include <vector>
#include <dlfcn.h>
#include "gl_context.hxx"
#include <glm/matrix.hpp>
#include "c_api.h"

class Context {
public:
  Context();
  ~Context();

  void rotate(glm::vec3 axis, float angle);
  void translate(glm::vec3 dir);
  void zoom(float f);
  void set_perspective(float fovy, float aspect, float znear, float zfar);

  template <typename T>
  T& get_property(const std::string& name, const T& default_v = T()) {
    return _property_store.get<T>(name, default_v);
  }

  template <typename T>
  void set_property(const std::string& name, const T& value){
    _property_store.set<T>(name, value);
  }

  void set_property_ptr(const std::string& name, void* value, size_t size);
  void *get_property_ptr(const std::string& name, void* default_val, size_t size);

  bool load_plugin(const std::string &plugin_name);
  egs_plugin_fun load_plugin_function(const std::string &plugin_name, const std::string &function_name);
  inline bool plugin_loaded(const std::string &plugin_name) {return loaded_plugins.find(plugin_name) != loaded_plugins.end();}
  static std::string get_plugin_path();

  void register_py_plugin_function(const std::string &plugin_name, const std::string &function_name, egs_plugin_fun fun);

  operator egs_context_ref() { return reinterpret_cast<egs_context_ref>(this); }
private:
  bool load_c_plugin(const std::string &plugin_file, const std::string &plugin_name);
  bool load_py_plugin(const std::string &plugin_file, const std::string &plugin_name);
  bool unload_c_plugin(const std::string &plugin_name);
  bool unload_py_plugin(const std::string &plugin_name);

  PropertyStore _property_store;
  std::unordered_map<std::string, deserializer_type> deserializers;
  std::unordered_map<std::string, void *> loaded_plugins;
  std::unordered_map<std::string, std::unordered_map<std::string, egs_plugin_fun>> py_plugin_functions;

  friend void egs_context_set_py_loader_fun(egs_context_ref, void (*)(egs_context_ref, const char *, const char *));
  void (*py_loader_fun)(egs_context_ref, const char *, const char *) = 0;
};

#endif
