#include "propertystore.hxx"

bool PropertyStore::contains(const std::string &name) {
  return (property_map.find(name) != property_map.end());
}

void *PropertyStore::get_ref(const std::string& name) {
  if (!contains(name)) {
    return nullptr;
  }
  return static_cast<void *>(property_map[name].get());
}

void PropertyStore::set_ptr(const std::string& name, void *value, size_t size) {
  property_map[name] = std::make_shared<CDataProperty>(value, size);
}

void* PropertyStore::get_ptr(const std::string& name, void* default_value, size_t size) {
  if (!contains(name)) {
    set_ptr(name, default_value, size);
  }
  IProperty *iproperty = property_map[name].get();
  assert(iproperty->size() == size);
  return iproperty->data();
}
