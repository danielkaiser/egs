#ifndef IPLUGIN_HXX
#define IPLUGIN_HXX

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <set>
#include "c_api.h"

class Context;
class GLContext;

class IDisplayListElement : public std::enable_shared_from_this<IDisplayListElement> {
public:
  virtual ~IDisplayListElement();
  virtual void apply(GLContext &ctx) = 0;

  void delete_function(GLContext &ctx);

  void register_delete_handler(GLContext &ctx);
  void unregister_delete_handler(GLContext &ctx);
protected:
  virtual void delete_handler(GLContext &ctx) = 0;
  std::set<GLContext*> registered_gl_contexts;
};

typedef std::unique_ptr<IDisplayListElement>(*deserializer_type)(const std::vector<uint8_t>::const_iterator& data, const std::vector<uint8_t>::const_iterator& data_end, Context &ctx);


class DisplayList : public IDisplayListElement {
public:
  virtual void apply(GLContext &ctx);
  virtual void delete_handler(GLContext &ctx) {}
  size_t size() {return display_list.size();}
  void remove(std::shared_ptr<IDisplayListElement> elem);
  
  std::shared_ptr<IDisplayListElement> add(std::shared_ptr<IDisplayListElement> dle);
  std::shared_ptr<IDisplayListElement> add(egs_display_list_elem_ref dle);
private:
  std::vector<std::shared_ptr<IDisplayListElement>> display_list;
};

template<typename T>
class PropertySetterDisplayListElement : public IDisplayListElement{
public:
  PropertySetterDisplayListElement(const std::string _name, const T& _value) : name(_name), value(_value) {}
  void update_value(const T& _value) {value = _value;}
  virtual void apply(GLContext &ctx);

private:
  const std::string name;
  T value;
};

#endif
