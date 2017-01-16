//
//  sphere_plugin.h
//  egs
//
//  Created by Daniel Kaiser on 15/05/16.
//  Copyright © 2016 Daniel Kaiser. All rights reserved.
//

#ifndef sphere_plugin_h
#define sphere_plugin_h

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "glip.h"
#include <GLFW/glfw3.h>

#include "egs.h"
#include "gen_sphere_vertices.h"

const char sphere_plugin_name[] = "sphere";
const char textured_sphere_plugin_name[] = "textured_sphere";

typedef struct {
  GLuint vao;
  GLuint vbo;
  GLuint ibo;
  GLuint instance_buffer;
} sphere_plugin_instance_data;

void sphere_plugin_init_plugin(egs_context_ref ctx_ref);
void sphere_plugin_terminate_plugin();

egs_display_list_elem_ref sphere_plugin_create_spheres(egs_context_ref ctx, unsigned int n, float *positions, float *radii, long *colors);
void sphere_plugin_destroy_spheres();
void sphere_apply(egs_gl_context_ref ctx, const size_t data_length, const uint8_t *data, c_property* instance_data);

egs_display_list_elem_ref sphere_plugin_create_textured_spheres(egs_context_ref ctx, unsigned int n, float *positions, float *radii);
void sphere_plugin_destroy_textured_spheres();
void textured_sphere_apply(egs_gl_context_ref ctx, const size_t data_length, const uint8_t *data, c_property* instance_data);
#endif /* sphere_plugin_h */
