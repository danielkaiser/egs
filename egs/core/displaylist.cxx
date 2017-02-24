#include "context.hxx"
#include "displaylist.hxx"
#include "gl_context.hxx"
#include <cassert>
#include <iostream>

void DisplayList::apply(GLContext &ctx) {
  int i=0;
  for (auto it : display_list) {
    egs_printf(EGS_DEBUG, "applying element %i\n", i++);
    it->register_delete_handler(ctx);
    it->apply(ctx);
  }
}

IDisplayListElement::~IDisplayListElement() {
  egs_printf(EGS_DEBUG, "IDisplayListElement destructor: deleting on delete handler on all contexts.\n");
  for (auto ctx : registered_gl_contexts) {
    std::function<void(GLContext &)> delete_func = std::bind(&IDisplayListElement::delete_function, this, std::placeholders::_1);
    egs_printf(EGS_DEBUG, "unregister delete handler for context %p\n", ctx);
    ctx->unregister_on_delete_handler(delete_func);
  }
}

void IDisplayListElement::register_delete_handler(GLContext &ctx) {
  std::function<void(GLContext &)> delete_func = std::bind(&IDisplayListElement::delete_function, this, std::placeholders::_1);
  ctx.register_on_delete_handler(delete_func);
  registered_gl_contexts.insert(&ctx);
}

void IDisplayListElement::unregister_delete_handler(GLContext &ctx) {
    std::function<void(GLContext &)> delete_func = std::bind(&IDisplayListElement::delete_function, this, std::placeholders::_1);
  ctx.unregister_on_delete_handler(delete_func);
  registered_gl_contexts.erase(&ctx);
}

void IDisplayListElement::delete_function(GLContext &ctx) {
  egs_printf(EGS_DEBUG, "removing DisplayListELement from GLContext %p\n", &ctx);
  registered_gl_contexts.erase(&ctx);
  egs_printf(EGS_DEBUG, "calling DisplayListELement delete_handler\n");
  this->delete_handler(ctx);
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

void egs_display_list_remove_element(egs_display_list_ref dl, egs_display_list_elem_ref elem) {
  auto tmp = std::shared_ptr<IDisplayListElement>(reinterpret_cast<IDisplayListElement*>(elem));
  reinterpret_cast<DisplayList *>(dl)->remove(tmp);
}

void egs_display_list_element_apply(egs_display_list_elem_ref dl, egs_gl_context_ref ctx) {
  reinterpret_cast<IDisplayListElement *>(dl)->apply(*reinterpret_cast<GLContext *>(ctx));
}

void egs_display_list_element_delete(egs_display_list_elem_ref dl, egs_gl_context_ref ctx) {
  reinterpret_cast<IDisplayListElement *>(dl)->delete_function(*reinterpret_cast<GLContext *>(ctx));
}

void egs_display_list_element_terminate(egs_display_list_elem_ref dl) {
  reinterpret_cast<IDisplayListElement *>(dl)->~IDisplayListElement();
}

template<typename T>
void PropertySetterDisplayListElement<T>::apply(GLContext &ctx) {
  ctx.set_property<T>(name, value);
}
