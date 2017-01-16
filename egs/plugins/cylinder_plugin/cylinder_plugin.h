//
//  cylinder_plugin.h
//  egs
//
//  Created by Daniel Kaiser on 18/05/16.
//  Copyright © 2016 Daniel Kaiser. All rights reserved.
//

#ifndef CYLINDER_PLUGIN_H
#define CYLINDER_PLUGIN_H

#include "c_api.h"

#ifdef __cplusplus
extern "C" {
#endif

void cylinder_plugin_init_plugin(egs_context_ref ctx);
void cylinder_plugin_terminate_plugin();
egs_display_list_elem_ref cylinder_plugin_create_cylinder(egs_context_ref ctx, int, float*, float*, float*, long);

#ifdef __cplusplus
}
#endif

#endif /* cylinder_plugin_h */
