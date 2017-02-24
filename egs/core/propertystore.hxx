#ifndef PROPERTYSTORE_HXX
#define PROPERTYSTORE_HXX

class PropertyStore;

#include <string>
#include <cassert>
#include <vector>
#include <memory>
#include <unordered_map>

class PropertyStore {
public:
  template <typename T>
  T& get(const std::string& name, const T& default_v = T());
  void *get_ref(const std::string& name);
  template <typename T>
  void set(const std::string& name, const T& value);
  void set_ptr(const std::string& name, void *value, size_t size);
  void* get_ptr(const std::string& name, void *default_value, size_t size);
  inline void clear() {property_map.clear();}
  
  bool contains(const std::string &name);
private:
  class IProperty {
  public:
    virtual void *data() = 0;
    virtual size_t size() = 0;
    virtual ~IProperty() {}
  };
  
  template <typename T>
  class Property: public IProperty {
  public:
    Property(const T& value): _value(value) {}
    T _value;
    void *data() {
      return &_value;
    };
    size_t size() {
      return sizeof(T);
    };
  };
  
  class CDataProperty : public IProperty {
  public:
    CDataProperty(void *ptr, size_t size);
    std::vector<unsigned char> _value;
    void *data() {
      return _value.data();
    };
    size_t size() {
      return _value.size();
    }
  };
  
  std::unordered_map<std::string, std::shared_ptr<IProperty>> property_map;
};

template<typename T>
T& PropertyStore::get(const std::string& name, const T& default_v) {
  if (!contains(name)) {
    set(name, default_v);
  }
  IProperty *iproperty = property_map[name].get();
  assert(sizeof(T) == iproperty->size());
  assert(dynamic_cast<CDataProperty *>(iproperty) || dynamic_cast<Property<T> *>(iproperty));
  if (dynamic_cast<CDataProperty *>(iproperty)) {
    property_map[name] = std::make_shared<Property<T>>(*(T *)iproperty->data());
  }
  return dynamic_cast<Property<T> *>(property_map[name].get())->_value;
}

template<typename T>
void PropertyStore::set(const std::string& name, const T& value) {
  property_map[name] = std::make_shared<Property<T>>(value);
}

#endif
