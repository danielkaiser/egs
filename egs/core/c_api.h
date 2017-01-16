//
//  c_api.h
//  egs
//
//  Created by Daniel Kaiser on 18/05/16.
//  Copyright © 2016 Daniel Kaiser. All rights reserved.
//

#ifndef C_API_H
#define C_API_H

#include "glip.h"

#ifdef __cplusplus
extern "C" {
#endif

/* propertystore */
typedef void *c_property;

/* context */
typedef struct egs_context_wrapper *egs_context_ref;
typedef struct egs_gl_context_wrapper *egs_gl_context_ref;

/* iplugin */
typedef struct egs_display_list_wrapper *egs_display_list_ref;
typedef struct egs_display_list_elem_wrapper *egs_display_list_elem_ref;

egs_display_list_ref egs_display_list_create();
void egs_display_list_add_element(egs_display_list_ref, egs_display_list_elem_ref);
void egs_display_list_element_apply(egs_display_list_elem_ref, egs_gl_context_ref);
void egs_display_list_element_terminate(egs_display_list_elem_ref);
void egs_display_list_destroy(egs_display_list_ref);

typedef egs_display_list_elem_ref(*egs_plugin_fun)(egs_context_ref, ...);

/* c_wrapper */
typedef struct {
  const char *plugin_name;
  const size_t data_length;
  const uint8_t *data;
} egs_cwrapper_t;

typedef egs_cwrapper_t* egs_cwrapper_ref;
typedef void (*egs_apply_fun)(egs_gl_context_ref, const size_t data_length, const uint8_t *data, c_property *instance_data);
typedef void (*egs_terminate_fun)(const size_t data_length, const uint8_t *data, c_property *instance_data);

void egs_c_wrapper_register_c_plugin(const char* plugin_name, egs_apply_fun apply, egs_terminate_fun term);
egs_display_list_elem_ref egs_c_wrapper_create(egs_cwrapper_t);

/* context */
egs_context_ref egs_context_create();
void egs_context_destroy(egs_context_ref);
void egs_context_load_plugin(egs_context_ref ctx_ref, const char *plugin_name);
void egs_context_register_py_plugin_function(egs_context_ref ctx_ref, const char *plugin_name, const char *function_name, void *fun);
void egs_context_set_py_loader_fun(egs_context_ref, void (*)(egs_context_ref, const char *, const char *));
void egs_context_set_property(egs_context_ref, const char *, void *, size_t);
void *egs_context_get_property(egs_context_ref, const char *, void *, size_t);
void egs_context_rotate(egs_context_ref, float x, float y, float z, float angle);
egs_plugin_fun egs_context_get_plugin_func(egs_context_ref ctx, const char *plugin, const char *func);

void egs_gl_context_set_property(egs_gl_context_ref, const char *, void *, size_t);
void *egs_gl_context_get_property(egs_gl_context_ref, const char *, void *, size_t);
egs_context_ref egs_gl_context_get_context(egs_gl_context_ref);
int egs_gl_context_update(egs_gl_context_ref, egs_display_list_ref);

/* gl_context */
typedef struct egs_glfw_context_wrapper *egs_glfw_context_ref;
egs_glfw_context_ref egs_glfw_context_create(egs_context_ref ctx);
void egs_glfw_context_destroy(egs_glfw_context_ref);

/* glip_context */
typedef struct egs_glip_glfw_context_wrapper *egs_glip_glfw_context_ref;
egs_glip_glfw_context_ref egs_glip_glfw_context_create(egs_context_ref ctx);
void egs_glip_glfw_context_destroy(egs_glip_glfw_context_ref);

/* offscreen context */
typedef struct egs_gloffscreen_context_wrapper *egs_gloffscreen_context_ref;
egs_gloffscreen_context_ref egs_gloffscreen_context_create(egs_context_ref ctx);
void egs_gloffscreen_context_destroy(egs_gloffscreen_context_ref);
int *egs_gloffscreen_get_data(egs_gloffscreen_context_ref);

/* compile_shader */
GLuint egs_util_compile_shader(const GLuint shader_type, const char *shader_src);
GLuint egs_util_compile_shader_file(const GLuint shader_type, const char *file_name);
GLuint egs_util_link_shader_program(const int n, const GLuint *shader, const char *frag_data_location);

typedef enum {EGS_DEBUG = 1, EGS_WARNING = 2, EGS_ERROR = 3} egs_msg_t;
void egs_printf(egs_msg_t message_type, const char *format_string, ...);

#ifdef __cplusplus
}
#endif

#endif /* c_api_h */
