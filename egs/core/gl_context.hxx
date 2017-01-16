#ifndef GL_CONTEXT_H
#define GL_CONTEXT_H

class DisplayList;
class GLContext;

#include "context.hxx"
#include <memory>
#include <iostream>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>

class GLContext : public std::enable_shared_from_this<GLContext> {
public:
  GLContext(Context &_ctx);
  virtual ~GLContext();
  virtual bool update(std::shared_ptr<DisplayList>) = 0;
  int draw_png(std::string filename, int width, int height);

  void rotate(glm::vec3 dir, float angle);
  void translate(glm::vec3 dir);
  void zoom(float f);
  void set_perspective(float fovy=45, float aspect=1, float znear=0.1, float zfar=200);
  void set_property_ptr(const std::string& name, void* value, size_t size);
  void *get_property_ptr(const std::string& name, void *default_val, size_t size);

  template <typename T>
  T& get_property(const std::string& name, const T& default_v = T()) {
    return property_store.get<T>(name, default_v);
  }

  template <typename T>
  void set_property(const std::string& name, const T& value){
    property_store.set<T>(name, value);
  }

  Context &get_context() {
    return ctx;
  }
protected:
  Context &ctx;
  PropertyStore property_store;
};

#endif
