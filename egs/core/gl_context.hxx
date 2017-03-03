#ifndef GL_CONTEXT_H
#define GL_CONTEXT_H

class DisplayList;
class GLContext;

#include "context.hxx"
#include <memory>
#include <iostream>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <unordered_set>
#include <set>

class GLContext : public std::enable_shared_from_this<GLContext> {
public:
  GLContext(Context &_ctx);
  GLContext(const GLContext &) = delete;
  virtual ~GLContext();
  virtual bool update(std::shared_ptr<DisplayList>) = 0;
  int draw_png(std::string filename, int width, int height);

  virtual int get_width() = 0;
  virtual int get_height() = 0;

  void register_on_delete_handler(const std::function<void(GLContext &)>&);
  void unregister_on_delete_handler(const std::function<void(GLContext &)>&);

  void rotate(glm::vec3 dir, float angle);
  void translate(glm::vec3 dir);
  void zoom(float f);
  void set_perspective(float fovy=45, float aspect=1, float znear=0.1, float zfar=200);
  void set_property_ptr(const std::string& name, void* value, size_t size);
  void *get_property_ptr(const std::string& name, void *default_val, size_t size);

  template <typename T>
  T& get_property(const std::string& name, const T& default_v = T()) {
    if (property_store.contains(name)) {
      return property_store.get<T>(name, default_v);
    }
    return ctx.get_property<T>(name, default_v);
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
private:
  struct func_compare {
    bool operator() (const std::function<void(GLContext &)>& lhs, const std::function<void(GLContext &)>& rhs) const {
      return lhs.target<void(GLContext &)>() < rhs.target<void(GLContext &)>();
    }
  };

  std::set<std::function<void(GLContext &)>, func_compare> on_delete_handler;
};

#endif
