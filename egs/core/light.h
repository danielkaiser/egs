//
//  light_plugin.h
//  egs
//
//  Created by Daniel Kaiser on 18/05/16.
//  Copyright © 2016 Daniel Kaiser. All rights reserved.
//

#ifndef LIGHT_PLUGIN_H
#define LIGHT_PLUGIN_H

#include "c_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct egs_directional_light {
  float direction_x;
  float direction_y;
  float direction_z;
  float ambient;
  float diffuse;
  float specular;
  long color;

} egs_directional_light_t;
  
#ifdef __cplusplus
class InitialisedDirectionalLight : public egs_directional_light {
public:
  InitialisedDirectionalLight() {
    direction_x = 0.3;
    direction_y = 0.3;
    direction_z = -1;
    ambient = 0.2;
    diffuse = 0.6;
    specular = 0.4;
    color = 0xffffffff;
  }
  InitialisedDirectionalLight(float direction_x, float direction_y, float direction_z, float ambient, float diffuse, float specular, long color) {
    this->direction_x = direction_x;
    this->direction_y = direction_y;
    this->direction_z = direction_z;
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular = specular;
    this->color = color;
  }
};
#endif

egs_directional_light_t egs_context_get_directional_light_property(egs_context_ref, const char *);

/*egs_display_list_elem_ref egs_light_plugin_create_directional_light(float, float, float);
egs_display_list_elem_ref egs_light_plugin_set_color(unsigned long);*/

#ifdef __cplusplus
}
#endif
  
#endif /* light_plugin_h */
