#include <memory>
#include <iostream>
#include <Python.h>
#include "context.hxx"
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "c_wrapper_dle.hxx"
#include "light.h"
#include <cstdio>
#if defined DEBUG || defined EBUG
  #include <signal.h>
  #include <execinfo.h>

  void segfault_debug(int x) {
    void* callstack[128];
    int i, frames = backtrace(callstack, 128);
    char** strs = backtrace_symbols(callstack, frames);
    fprintf(stderr, "SEGFAULT handler (signal %d)\n", x);
    for (i = 0; i < frames; ++i) {
      fprintf(stderr, "%s\n", strs[i]);
    }
    free(strs);
    exit(EXIT_FAILURE);
  }
#endif

Context::Context() {
  set_property<egs_directional_light_t>("light::directional_light", InitialisedDirectionalLight());
  set_perspective(45.0, 1.0, 0.1, 200);
  glm::vec3 camera_position = {0, 0, 10};
  glm::vec3 camera_center = {0, 0, 0};
  glm::vec3 camera_up = {0, 1, 0};
  set_property("camera_position", camera_position);
  set_property("camera_center", camera_center);
  set_property("camera_up", camera_up);
  set_property("view_matrix", glm::lookAt(camera_position, camera_center, camera_up));
  set_property("clear_color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

#if defined DEBUG || defined EBUG
  struct sigaction sa;
  sa.sa_handler = &segfault_debug;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGSEGV, &sa, NULL);
  sigaction(SIGINT, &sa, NULL);
#endif
}

Context::~Context() {
  _property_store.clear();
  for (auto dl_handle: loaded_plugins) {
    if (dl_handle.second) {
      egs_printf(EGS_DEBUG, "unloading c plugin %s (%p)\n", dl_handle.first.c_str(), dl_handle.second);
      unload_c_plugin(dl_handle.first);
      dlclose(dl_handle.second);
    } else {
      egs_printf(EGS_DEBUG, "unloading py plugin %s\n", dl_handle.first.c_str());
      unload_py_plugin(dl_handle.first);
    }
  }
  if (Py_IsInitialized()) {
    Py_Finalize();
  }
}

void Context::rotate(glm::vec3 axis, float angle) {
  auto camera_position = get_property<glm::vec3>("camera_position");
  auto camera_center = get_property<glm::vec3>("camera_center");
  auto camera_up = get_property<glm::vec3>("camera_up");
  auto camera_forward = camera_center-camera_position;
  auto camera_right = glm::normalize(glm::cross(camera_forward, camera_up));

  axis = axis.x * camera_right + axis.y * camera_up + axis.z * camera_forward;
  camera_position = glm::vec3(glm::rotate(angle, axis)*glm::vec4(camera_position, 1));
  camera_up = glm::normalize(glm::cross(camera_right, camera_center-camera_position));
  set_property("camera_position", camera_position);
  set_property("camera_center", camera_center);
  set_property("camera_up", camera_up);
  set_property("view_matrix", glm::lookAt(camera_position, camera_center, camera_up));
}

void Context::translate(glm::vec3 dir) {
  auto camera_position = get_property<glm::vec3>("camera_position");
  auto camera_center = get_property<glm::vec3>("camera_center");
  auto camera_up = get_property<glm::vec3>("camera_up");
  auto camera_forward = camera_center-camera_position;
  auto camera_right = glm::normalize(glm::cross(camera_forward, camera_up));
  dir = dir.x * camera_right + dir.y * camera_up + dir.z * camera_forward;
  camera_center += dir;
  camera_position += dir;
  set_property("camera_position", camera_position);
  set_property("camera_center", camera_center);
  auto view_matrix = glm::lookAt(camera_position, camera_center, camera_up);
  set_property("view_matrix", view_matrix);
}

void Context::zoom(float f) {
  auto camera_position = get_property<glm::vec3>("camera_position");
  auto camera_center = get_property<glm::vec3>("camera_center");
  auto camera_up = get_property<glm::vec3>("camera_up");
  camera_position = (camera_position - camera_center)*f + camera_center;
  set_property("camera_position", camera_position);
  set_property("view_matrix", glm::lookAt(camera_position, camera_center, camera_up));
}

void Context::set_perspective(float fovy, float aspect, float znear, float zfar) {
  set_property<glm::mat4>("projection", glm::perspective(fovy, aspect, znear, zfar));
}

bool Context::load_c_plugin(const std::string &plugin_file, const std::string &plugin_name) {
  void *dl_handle;
  dl_handle = dlopen(plugin_file.c_str(), RTLD_LAZY);
  loaded_plugins[plugin_name] = dl_handle;
  if (!dl_handle) {
    egs_printf(EGS_ERROR, "Error while loading plugin: %s\n", dlerror());
    return false;
  } else {
    std::string init_function_name = plugin_name + "_init_plugin";
    auto init_fn = dlsym(dl_handle, init_function_name.c_str());
    if (!init_fn) {
      egs_printf(EGS_ERROR, "Error while initializing plugin: %s", dlerror());
    } else {
      (reinterpret_cast<void (*)(Context &)>(init_fn))(*this);
      return true;
    }
    return false;
  }
}

bool Context::unload_c_plugin(const std::string &plugin_name) {
  if (loaded_plugins.find(plugin_name) == loaded_plugins.end()) {
    egs_printf(EGS_WARNING, "Context::unload_c_plugin: Plugin %s not loaded\n", plugin_name.c_str());
    return false;
  }
  std::string term_function_name = plugin_name + "_terminate_plugin";
  auto term_fn = dlsym(loaded_plugins[plugin_name], term_function_name.c_str());
  if (!term_fn) {
    egs_printf(EGS_ERROR, "Error while terminating plugin: %s\n", dlerror());
  } else {
    (reinterpret_cast<void (*)()>(term_fn))();
    return true;
  }
  return false;
}

void egs_context_set_py_loader_fun(egs_context_ref ctx_ref, void (*fun)(egs_context_ref, const char *, const char *)) {
  reinterpret_cast<Context *>(ctx_ref)->py_loader_fun = fun;
}

bool Context::load_py_plugin(const std::string &plugin_path, const std::string &plugin_name) {
  if (py_loader_fun) {
    py_loader_fun(reinterpret_cast<egs_context_ref>(this), plugin_path.c_str(), plugin_name.c_str());
    loaded_plugins[plugin_name] = nullptr;
    return true;
  }
  std::string init_function_name = plugin_name + "_init_plugin";
  PyObject *pName, *pModule, *pFunc, *pValue, *pArgs;

  if (getenv("PYTHONPATH")) {
    setenv("PYTHONPATH", (std::string(getenv("PYTHONPATH"))+":"+plugin_path+"/"+plugin_name).c_str(), 1);
  } else {
    setenv("PYTHONPATH", (std::string(getenv("EGS_PATH"))+"/python").c_str(), 0);
    setenv("PYTHONPATH", (std::string(getenv("PYTHONPATH"))+":"+plugin_path+"/"+plugin_name).c_str(), 1);
  }

  egs_printf(EGS_DEBUG, "python state before Py_Initialize: %d\n", Py_IsInitialized());
  egs_printf(EGS_DEBUG, "PYTHONPATH: %s\n", getenv("PYTHONPATH"));
  if (!Py_IsInitialized()) {
    Py_Initialize();
  }

  PyGILState_STATE py_gilstate;
  py_gilstate = PyGILState_Ensure();
#if PY_MAJOR_VERSION >= 3
  pName = PyUnicode_FromString(plugin_name.c_str());
#else
  pName = PyString_FromString(plugin_name.c_str());
#endif
  pModule = PyImport_Import(pName);

  Py_DECREF(pName);

  if (pModule != NULL) {
    pFunc = PyObject_GetAttrString(pModule, init_function_name.c_str());

    if (pFunc && PyCallable_Check(pFunc)) {
      pArgs = PyTuple_New(1);
      //pValue = PyCapsule_New(this, "Context", NULL);
#if PY_MAJOR_VERSION >= 3
      pValue = PyLong_FromLong((long)this);
#else
      pValue = PyInt_FromLong((long)this);
#endif
      if (!pValue) {
        Py_DECREF(pArgs);
        Py_DECREF(pModule);
        std::cerr << "Cannot convert argument" << std::endl;
        return 1;
      }
      PyTuple_SetItem(pArgs, 0, pValue);

      pValue = PyObject_CallObject(pFunc, pArgs);
      if (pValue != NULL) {
#if PY_MAJOR_VERSION >= 3
        long result = PyLong_AsLong(pValue);
#else
        long result = PyInt_AsLong(pValue);
#endif
        Py_DECREF(pValue);
        if (!result) {
          Py_DECREF(pFunc);
          Py_DECREF(pModule);
        }
        loaded_plugins[plugin_name] = nullptr;
      }
      else {
        Py_DECREF(pFunc);
        Py_DECREF(pModule);
        PyErr_Print();
        std::cerr << "Python plugin init failed" << std::endl;
        return false;
      }
    } else {
      if (PyErr_Occurred())
        PyErr_Print();
      std::cerr << "Cannot find plugin init function " << init_function_name << std::endl;
    }
    Py_XDECREF(pFunc);
    Py_DECREF(pModule);
  } else {
    PyErr_Print();
    std::cerr << "Failed to load plugin " << plugin_path << " " << plugin_name << std::endl;
    return false;
  }
  PyGILState_Release(py_gilstate);
  return true;
}

bool Context::unload_py_plugin(const std::string &plugin_name) {
  if (py_loader_fun) {
    py_loader_fun(reinterpret_cast<egs_context_ref>(this), "", plugin_name.c_str());
    return true;
  } else {
    std::string term_function_name = plugin_name + "_terminate_plugin";
    PyObject *pName, *pModule, *pFunc, *pValue, *pArgs;

    if (!Py_IsInitialized()) {
      egs_printf(EGS_WARNING, "Unloading plugin %s: Python interpreter not initialized.\n");
      return false;
    }

    PyGILState_STATE py_gilstate;
    py_gilstate = PyGILState_Ensure();
#if PY_MAJOR_VERSION >= 3
    pName = PyUnicode_FromString(plugin_name.c_str());
#else
    pName = PyString_FromString(plugin_name.c_str());
#endif

    pModule = PyImport_Import(pName);

    Py_DECREF(pName);

    if (pModule != NULL) {
      pFunc = PyObject_GetAttrString(pModule, term_function_name.c_str());

      if (pFunc && PyCallable_Check(pFunc)) {
        pArgs = PyTuple_New(0);
        pValue = PyObject_CallObject(pFunc, pArgs);
        Py_DECREF(pArgs);
        Py_DECREF(pFunc);
        Py_DECREF(pModule);
      } else {
        if (PyErr_Occurred())
          PyErr_Print();
        std::cerr << "Cannot find plugin terminate function " << term_function_name << std::endl;
      }
      Py_XDECREF(pFunc);
      Py_DECREF(pModule);
    } else {
      PyErr_Print();
      std::cerr << "Failed to find plugin " << plugin_name << std::endl;
      return false;
    }
    PyGILState_Release(py_gilstate);
    return true;
  }
}

bool Context::load_plugin(const std::string &plugin_name) {
  if (loaded_plugins.find(plugin_name) == loaded_plugins.end()) {
    std::string plugin_path = Context::get_plugin_path();

    std::string c_plugin = plugin_path + "/" + plugin_name + "/lib" + plugin_name +".so";
    std::string c_plugin_mac = plugin_path + "/" + plugin_name + "/lib" + plugin_name +".dylib";
    std::string py_plugin = plugin_path + "/" + plugin_name + "/" + plugin_name + ".py";
    struct stat buffer;
    if (stat (c_plugin_mac.c_str(), &buffer) == 0) {
      egs_printf(EGS_DEBUG, "loading dylib: %s\n", c_plugin_mac.c_str());
      return load_c_plugin(c_plugin_mac, plugin_name);
    } else if (stat (c_plugin.c_str(), &buffer) == 0) {
      egs_printf(EGS_DEBUG, "loading so: %s\n", c_plugin.c_str());
      return load_c_plugin(c_plugin, plugin_name);
    } else if (stat (py_plugin.c_str(), &buffer) == 0) {
      egs_printf(EGS_DEBUG, "loading py: %s\n", py_plugin.c_str());
      return load_py_plugin(plugin_path, plugin_name);
    }
    egs_printf(EGS_DEBUG, "could not find plugin file for: %s\n", plugin_name.c_str());
    return false;
  }
  return true;
}

std::string Context::get_plugin_path() {
  std::string plugin_path;
  if (!getenv("EGS_PLUGIN_PATH")) {
    if (getenv("EGS_PATH")) {
      plugin_path = std::string(getenv("EGS_PATH"))+"/plugins";
    } else {
      plugin_path = "./plugins";
    }
  } else {
    plugin_path = getenv("EGS_PLUGIN_PATH");
  }
  return plugin_path;
}

void Context::register_py_plugin_function(const std::string &plugin_name, const std::string &function_name, egs_plugin_fun fun){
  if (py_plugin_functions.find(plugin_name) == py_plugin_functions.end()) {
    py_plugin_functions[plugin_name] = std::unordered_map<std::string, egs_plugin_fun>();
  }
  py_plugin_functions[plugin_name][function_name] = fun;
}


void Context::set_property_ptr(const std::string& name, void* value, size_t size) {
  this->_property_store.set_ptr(name, value, size);
}

void *Context::get_property_ptr(const std::string& name, void *default_val, size_t size) {
  return this->_property_store.get_ptr(name, default_val, size);
}

egs_context_ref egs_context_create() {
  Context *ctx = new Context;
  return reinterpret_cast<egs_context_ref>(ctx);
}

void egs_context_destroy(egs_context_ref ctx) {
  delete reinterpret_cast<Context *>(ctx);
}

void egs_context_load_plugin(egs_context_ref ctx_ref, const char *plugin_name) {
  reinterpret_cast<Context*>(ctx_ref)->load_plugin(std::string(plugin_name));
}

void egs_context_set_property(egs_context_ref ctx, const char *name, void *data, size_t size) {
  reinterpret_cast<Context *>(ctx)->set_property_ptr(name, data, size);
}

void *egs_context_get_property(egs_context_ref ctx, const char *name, void *default_val, size_t size) {
  return reinterpret_cast<Context *>(ctx)->get_property_ptr(name, default_val, size);
}

void egs_context_rotate(egs_context_ref ctx, float x, float y, float z, float angle) {
  reinterpret_cast<Context *>(ctx)->rotate(glm::vec3(x, y, z), angle);
}

egs_plugin_fun Context::load_plugin_function(const std::string &plugin_name, const std::string &function_name) {
  if(!plugin_loaded(plugin_name)) {
    if (!load_plugin(plugin_name)) {
      egs_printf(EGS_ERROR, "plugin %s not found.\n", plugin_name.c_str());
      return nullptr;
    }
  }
  if (loaded_plugins[plugin_name] == nullptr) { /* python plugin */

    return py_plugin_functions[plugin_name][function_name];
  }
  void *fun_ptr;
  fun_ptr = dlsym(loaded_plugins[plugin_name], function_name.c_str());
  assert(fun_ptr);
  std::cout<< fun_ptr <<std::endl;
  return (egs_plugin_fun)fun_ptr;
}

egs_plugin_fun egs_context_get_plugin_func(egs_context_ref ctx, const char *plugin, const char *func) {
  return reinterpret_cast<Context *>(ctx)->load_plugin_function(plugin, func);
}

void egs_context_register_py_plugin_function(egs_context_ref ctx, const char *plugin_name, const char *function_name, void *fun) {
  reinterpret_cast<Context *>(ctx)->register_py_plugin_function(plugin_name, function_name, (egs_plugin_fun)fun);
}

void egs_printf(egs_msg_t msg_type, const char *format_string, ...) {
  switch (msg_type) {
    case EGS_DEBUG:
      #if defined DEBUG || defined EBUG
      fprintf(stderr, "[DEBUG] ");
      break;
      #else
        return;
      #endif
    case EGS_WARNING:
      fprintf(stderr, "[WARNING] ");
      break;
    case EGS_ERROR:
      fprintf(stderr, "[ERROR] ");
      break;
  }
  va_list args;
  va_start(args, format_string);
  va_end(args);
  vfprintf(stderr, format_string, args);
}

