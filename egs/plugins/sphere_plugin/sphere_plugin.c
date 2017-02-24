//
//  sphere_plugin.c
//  egs
//
//  Created by Daniel Kaiser on 15/05/16.
//  Copyright © 2016 Daniel Kaiser. All rights reserved.
//
#include <math.h>
#include "sphere_plugin.h"

#define SPHERE_SUBDIVISION_LEVEL 3

typedef struct {
  GLuint vao, vbo, ibo, instance_buffer;
} sphere_instance_t;

void sphere_apply(egs_gl_context_ref ctx, const size_t data_length, const uint8_t *data, c_property* instance_data, c_property* per_context_instance_data) {
  assert(data_length % (7*sizeof(float)) == 0);

  GLint shader_prog = -1;
  if ((shader_prog = *(int *)egs_gl_context_get_property(ctx, "sphere::shader_program", (void *)&shader_prog, sizeof(int))) == -1) {
    GLuint compiled_shader[2];
    compiled_shader[0] = egs_util_compile_shader_file(GL_VERTEX_SHADER, "/sphere_plugin/vert.glsl");
    compiled_shader[1] = egs_util_compile_shader_file(GL_FRAGMENT_SHADER, "/sphere_plugin/frag.glsl");
    shader_prog = egs_util_link_shader_program(2, compiled_shader, "fragment_color");
    egs_gl_context_set_property(ctx, "sphere::shader_program", &shader_prog, sizeof(int));
    assert(!glGetError());
  }
  glUseProgram(shader_prog);

  GLfloat *vertices;
  GLuint *indices;
  int n_vert, n_elem;

  if (!*per_context_instance_data) {
    *per_context_instance_data = (c_property)malloc(sizeof(sphere_instance_t));
    sphere_instance_t* sphere_instance = (sphere_instance_t*)*per_context_instance_data;
    assert(glGetError() == 0);
    glGenBuffers(1, &sphere_instance->vbo);
    glGenBuffers(1, &sphere_instance->ibo);
    glGenVertexArrays(1, &sphere_instance->vao);
    glBindVertexArray(sphere_instance->vao);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_instance->vbo);

    assert(glGetError() == 0);
    glVertexAttribPointer(glGetAttribLocation(shader_prog, "position"), 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    assert(glGetError() == 0);
    create_sphere_vertices(SPHERE_SUBDIVISION_LEVEL, &n_vert, &vertices, &n_elem, &indices);
    egs_printf(EGS_DEBUG, "nvert: %d, nind: %d\n", n_vert, n_elem);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_instance->vbo);
    glBufferData(GL_ARRAY_BUFFER, 3*n_vert*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_instance->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*n_elem*sizeof(GLuint), indices, GL_STATIC_DRAW);
    free(vertices);
    free(indices);

    glGenBuffers(1, &sphere_instance->instance_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_instance->instance_buffer);
    glEnableVertexAttribArray(glGetAttribLocation(shader_prog, "sphere_position"));
    glVertexAttribPointer(glGetAttribLocation(shader_prog, "sphere_position"), 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), NULL);
    glVertexAttribDivisor(glGetAttribLocation(shader_prog, "sphere_position"), 1);
    assert(glGetError() == 0);
    glEnableVertexAttribArray(glGetAttribLocation(shader_prog, "sphere_color"));
    glVertexAttribPointer(glGetAttribLocation(shader_prog, "sphere_color"), 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void *)(4  * sizeof(float)));
    glVertexAttribDivisor(glGetAttribLocation(shader_prog, "sphere_color"), 1);
    assert(glGetError() == 0);
    glBufferData(GL_ARRAY_BUFFER, data_length, (void*)data, GL_STATIC_DRAW);
    assert(glGetError() == 0);

    egs_printf(EGS_DEBUG, "created sphere buffer\n");
  }

  sphere_instance_t* sphere_instance = (sphere_instance_t*)*per_context_instance_data;
  assert(glGetError() == 0);
  glBindVertexArray(sphere_instance->vao);
  egs_directional_light_t light = egs_context_get_directional_light_property(egs_gl_context_get_context(ctx), "light::directional_light");
  glUniform3f(glGetUniformLocation(shader_prog, "light_direction"), light.direction_x, light.direction_y, light.direction_z);
  glUniformMatrix4fv(glGetUniformLocation(shader_prog, "view_matrix"), 1, GL_FALSE, (float *)egs_gl_context_get_property(ctx, "view_matrix", NULL, 16*sizeof(float)));
  glUniformMatrix4fv(glGetUniformLocation(shader_prog, "projection"), 1, GL_FALSE, (float *)egs_gl_context_get_property(ctx, "projection", NULL, 16*sizeof(float)));

  assert(glGetError() == 0);
  glDrawElementsInstanced(GL_TRIANGLES, 3*number_of_elements(SPHERE_SUBDIVISION_LEVEL), GL_UNSIGNED_INT, NULL, (GLsizei)data_length/(7*sizeof(float)));
  assert(glGetError() == 0);
}

void sphere_plugin_init_plugin(egs_context_ref ctx_ref) {
  (void) ctx_ref;
  egs_c_wrapper_register_c_plugin(sphere_plugin_name, &sphere_apply, &sphere_plugin_destroy_spheres, &sphere_plugin_terminate_spheres);
  egs_c_wrapper_register_c_plugin(textured_sphere_plugin_name, &textured_sphere_apply, &sphere_plugin_destroy_textured_spheres, &sphere_plugin_terminate_textured_spheres);
}

void sphere_plugin_terminate_plugin() {
  egs_printf(EGS_DEBUG, "sphere plugin terminated\n");
}

egs_display_list_elem_ref sphere_plugin_create_spheres(egs_context_ref ctx, unsigned int n, float *positions, float *radii, long *colors) {
  (void) ctx;
  unsigned int i;
  float *data = (float *)malloc(n*7*sizeof(float));
  assert(data);
  for (i=0; i<n; i++) {
    data[7*i+0] = positions[3*i+0];
    data[7*i+1] = positions[3*i+1];
    data[7*i+2] = positions[3*i+2];
    data[7*i+3] = radii[i];
    data[7*i+4] = ((colors[i]&0x00ff0000)>>16)/255.;
    data[7*i+5] = ((colors[i]&0x0000ff00)>>8)/255.;
    data[7*i+6] = ((colors[i]&0x000000ff))/255.;
  }
  egs_cwrapper_t wrapper = {sphere_plugin_name, n*7*sizeof(float), (uint8_t*)data};
  return egs_c_wrapper_create(wrapper);
}

void sphere_plugin_destroy_spheres(egs_gl_context_ref ctx, const size_t data_length, const uint8_t *data, c_property* instance_data, c_property* per_context_instance_data) {
  (void) data_length;
  (void) data;
  if (*per_context_instance_data) {
    sphere_instance_t* sphere_instance = (sphere_instance_t*)*per_context_instance_data;
    egs_printf(EGS_DEBUG, "Deleting: vbo %d ; ibo %d ; instance buffer %d ; vao %d\n", sphere_instance->vbo, sphere_instance->ibo, sphere_instance->instance_buffer, sphere_instance->vao);
    const GLuint buffers[] = {sphere_instance->vbo, sphere_instance->ibo, sphere_instance->instance_buffer};
    glDeleteBuffers(3, buffers);
    glDeleteVertexArrays(1, &sphere_instance->vao);
    free(*per_context_instance_data);
    *per_context_instance_data = NULL;
  }
  egs_printf(EGS_DEBUG, "sphere deleted\n");
}

void sphere_plugin_terminate_spheres(const size_t data_length, const uint8_t *data, c_property* instance_data) {
  free((void*)data);
  egs_printf(EGS_DEBUG, "sphere terminated\n");
}

/******************************************************************************************/
/** Textured sphere ***********************************************************************/

void textured_sphere_apply(egs_gl_context_ref ctx, const size_t data_length, const uint8_t *data, c_property* instance_data, c_property* Per_context_instance_data) {
  GLint shader_prog = -1;
  if ((shader_prog = *(int *)egs_gl_context_get_property(ctx, "sphere::shader_program", (void *)&shader_prog, sizeof(int))) == -1) {
    GLuint compiled_shader[2];
    compiled_shader[0] = egs_util_compile_shader_file(GL_VERTEX_SHADER, "/sphere_plugin/texture_vert.glsl");
    compiled_shader[1] = egs_util_compile_shader_file(GL_FRAGMENT_SHADER, "/sphere_plugin/texture_frag.glsl");
    shader_prog = egs_util_link_shader_program(2, compiled_shader, "fragment_color");
    egs_gl_context_set_property(ctx, "sphere::shader_program", &shader_prog, sizeof(int));
    glGetError();
  }
  glUseProgram(shader_prog);

  if (!*instance_data) {
    GLfloat *vertices;
    GLuint *indices;
    int n_vert, n_elem, i;
    *instance_data = (c_property)malloc(sizeof(sphere_instance_t));
    sphere_instance_t* sphere_instance = (sphere_instance_t*)instance_data;

    assert(glGetError() == 0);
    glGenBuffers(1, &sphere_instance->vbo);
    glGenVertexArrays(1, &sphere_instance->vao);
    glBindVertexArray(sphere_instance->vao);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_instance->vbo);

    assert(glGetError() == 0);
    glVertexAttribPointer(glGetAttribLocation(shader_prog, "position"), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL);
    glVertexAttribPointer(glGetAttribLocation(shader_prog, "texture_position"), 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3  * sizeof(float)));
    glEnableVertexAttribArray(glGetAttribLocation(shader_prog, "position"));
    glEnableVertexAttribArray(glGetAttribLocation(shader_prog, "texture_position"));
    assert(glGetError() == 0);
    create_sphere_vertices(SPHERE_SUBDIVISION_LEVEL+1, &n_vert, &vertices, &n_elem, &indices);
    egs_printf(EGS_DEBUG, "nvert: %d, nind: %d\n", n_vert, n_elem);
    float *vert = (float *) malloc(5*3*n_elem*sizeof(GLfloat));
    for (i=0; i<3*n_elem; i++) {
      vert[5*i+0] = vertices[3*indices[i]+0];
      vert[5*i+1] = vertices[3*indices[i]+1];
      vert[5*i+2] = vertices[3*indices[i]+2];
      vert[5*i+3] = 0.5+atan2(vertices[3*indices[i]+0], vertices[3*indices[i]+2])/(2*3.14156);
      vert[5*i+4] = acos(vertices[3*indices[i]+1])/3.14156;
    }
    for (i=0; i<n_elem; i++) {
      if (fabs(vert[5*(3*i+0)+3] - vert[5*(3*i+1)+3]) > 0.5 || fabs(vert[5*(3*i+0)+3] - vert[5*(3*i+2)+3]) > 0.5) {
        if (vert[5*(3*i+0)+3] > 0.5) {
          vert[5*(3*i+0)+3] -= 1;
        }
        if (vert[5*(3*i+1)+3] > 0.5) {
          vert[5*(3*i+1)+3] -= 1;
        }
        if (vert[5*(3*i+2)+3] > 0.5) {
          vert[5*(3*i+2)+3] -= 1;
        }
      }
    }
    glBindBuffer(GL_ARRAY_BUFFER, sphere_instance->vbo);
    glBufferData(GL_ARRAY_BUFFER, 5*3*n_elem*sizeof(GLfloat), vert, GL_STATIC_DRAW);
    free(vert);
    free(vertices);
    free(indices);

    glGenBuffers(1, &sphere_instance->instance_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_instance->instance_buffer);
    glEnableVertexAttribArray(glGetAttribLocation(shader_prog, "sphere_position"));
    glVertexAttribPointer(glGetAttribLocation(shader_prog, "sphere_position"), 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribDivisor(glGetAttribLocation(shader_prog, "sphere_position"), 1);
    assert(glGetError() == 0);
    glBufferData(GL_ARRAY_BUFFER, data_length, (void*)data, GL_STATIC_DRAW);
    assert(glGetError() == 0);

    egs_printf(EGS_DEBUG, "created textured sphere buffer\n");
  }
  assert(glGetError() == 0);
  glBindVertexArray(((sphere_instance_t*)instance_data)->vao);
  egs_directional_light_t light = egs_context_get_directional_light_property(egs_gl_context_get_context(ctx), "light::directional_light");
  glUniform1i(glGetUniformLocation(shader_prog, "rendered_texture"), 0);
  glUniform3f(glGetUniformLocation(shader_prog, "light_direction"), light.direction_x, light.direction_y, light.direction_z);
  glUniformMatrix4fv(glGetUniformLocation(shader_prog, "view_matrix"), 1, GL_FALSE, (float *)egs_gl_context_get_property(ctx, "view_matrix", NULL, 16*sizeof(float)));
  glUniformMatrix4fv(glGetUniformLocation(shader_prog, "projection"), 1, GL_FALSE, (float *)egs_gl_context_get_property(ctx, "projection", NULL, 16*sizeof(float)));

  assert(glGetError() == 0);
  glDrawArraysInstanced(GL_TRIANGLES, 0, 3*number_of_elements(SPHERE_SUBDIVISION_LEVEL+1), (GLsizei)data_length/(4*sizeof(float)));
  assert(glGetError() == 0);
}

egs_display_list_elem_ref sphere_plugin_create_textured_spheres(egs_context_ref ctx, unsigned int n, float *positions, float *radii) {
  (void) ctx;
  unsigned int i;
  float *data = malloc(n*4*sizeof(float));
  assert(data);
  for (i=0; i<n; i++) {
    data[4*i+0] = positions[3*i+0];
    data[4*i+1] = positions[3*i+1];
    data[4*i+2] = positions[3*i+2];
    data[4*i+3] = radii[i];
  }
  egs_cwrapper_t wrapper = {textured_sphere_plugin_name, n*4*sizeof(float), (uint8_t*)data};
  return egs_c_wrapper_create(wrapper);
}

void sphere_plugin_destroy_textured_spheres(egs_gl_context_ref ctx, const size_t data_length, const uint8_t *data, c_property* instance_data, c_property* per_context_instance_data) {
  (void) data_length;
  (void) data;
  if (*per_context_instance_data) {
    sphere_instance_t* sphere_instance = (sphere_instance_t*)*per_context_instance_data;
    egs_printf(EGS_DEBUG, "Deleting: vbo %d ; instance buffer %d ; vao %d\n", sphere_instance->vbo, sphere_instance->ibo, sphere_instance->instance_buffer, sphere_instance->vao);
    const GLuint buffers[] = {sphere_instance->vbo, sphere_instance->instance_buffer};
    glDeleteBuffers(2, buffers);
    glDeleteVertexArrays(1, &sphere_instance->vao);
    free(*per_context_instance_data);
    *per_context_instance_data = NULL;
  }
  egs_printf(EGS_DEBUG, "textured sphere deleted\n");
}

void sphere_plugin_terminate_textured_spheres(const size_t data_length, const uint8_t *data, c_property* instance_data) {
  free((void*)data);
  egs_printf(EGS_DEBUG, "textured sphere terminated\n");
}
