#include "context.hxx"
#include "displaylist.hxx"
#include <cassert>
#include <iostream>

void DisplayList::apply(GLContext &ctx) {
  int i=0;
  for (auto it : display_list) {
    egs_printf(EGS_DEBUG, "applying element %i\n", i++);
    it->apply(ctx);
  }
}

std::shared_ptr<IDisplayListElement> DisplayList::add(std::shared_ptr<IDisplayListElement> dle) {
  display_list.push_back(dle);
  return dle;
}

std::shared_ptr<IDisplayListElement> DisplayList::add(egs_display_list_elem_ref dle) {
  return add(std::shared_ptr<IDisplayListElement>(reinterpret_cast<IDisplayListElement *>(dle)));
}

void DisplayList::remove(std::shared_ptr<IDisplayListElement> elem) {
  for (auto elem_it = display_list.begin(); elem_it < display_list.end(); elem_it++) {
    if (*elem_it == elem) {
      display_list.erase(elem_it);
      return;
    }
  }
}

egs_display_list_ref egs_display_list_create() {
  DisplayList *dl = new DisplayList;
  return reinterpret_cast<egs_display_list_ref>(dl);
}

void egs_display_list_destroy(egs_display_list_ref dl) {
  delete reinterpret_cast<DisplayList *>(dl);
}

void egs_display_list_add_element(egs_display_list_ref dl, egs_display_list_elem_ref elem) {
  auto tmp = std::shared_ptr<IDisplayListElement>(reinterpret_cast<IDisplayListElement*>(elem));
  reinterpret_cast<DisplayList *>(dl)->add(tmp);
}

template<typename T>
void PropertySetterDisplayListElement<T>::apply(GLContext &ctx) {
  ctx.set_property<T>(name, value);
}
